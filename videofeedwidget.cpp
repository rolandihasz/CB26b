#include "videofeedwidget.h"

#include <QPainter>
#include <QMediaDevices>
#include <QCameraDevice>
#include <QScreen>
#include <QGuiApplication>
#include <QMenu>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QDebug>
#include <cmath>

VideoFeedWidget::VideoFeedWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(640, 360);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setMouseTracking(true);
    loadSettings();

    m_videoSink = new QVideoSink(this);
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &VideoFeedWidget::onVideoFrameChanged);

    m_captureSession = new QMediaCaptureSession(this);
    m_captureSession->setVideoSink(m_videoSink);

    m_dshowCapture = new DirectShowCapture(this);
    connect(m_dshowCapture, &DirectShowCapture::frameReady, this, &VideoFeedWidget::onDirectShowFrame);
    connect(m_dshowCapture, &DirectShowCapture::statusMessage, this, &VideoFeedWidget::statusMessage);

    // Setup Video Player for pre-recorded gameplay analysis
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setVolume(0.5f);
    m_player->setAudioOutput(m_audioOutput);

    connect(m_player, &QMediaPlayer::positionChanged, this, [this](qint64 pos) {
        m_videoPositionMs = pos;
        emit videoPositionChanged(pos, m_videoDurationMs);
        update();
    });

    connect(m_player, &QMediaPlayer::durationChanged, this, [this](qint64 dur) {
        m_videoDurationMs = dur;
        emit videoDurationChanged(dur);
    });

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
        emit playbackStateChanged(state == QMediaPlayer::PlayingState);
        update();
    });

    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString) {
        Q_UNUSED(error);
        emit statusMessage(QStringLiteral("Media player error: %1").arg(errorString));
    });

    m_fpsTimer.start();
    m_lapTimer.start();

    // AI Inference timer (run ~5 times per second at 1x)
    connect(&m_inferenceTimer, &QTimer::timeout, this, &VideoFeedWidget::onInferenceTimer);
    m_inferenceTimer.start(200);

    // Blinking timer for LIVE badge
    connect(&m_blinkTimer, &QTimer::timeout, this, [this]() {
        m_liveBlinkState = !m_liveBlinkState;
        update();
    });
    m_blinkTimer.start(800);

    // Fallback timer in case camera is offline
    connect(&m_fallbackTimer, &QTimer::timeout, this, &VideoFeedWidget::onScreenCaptureFallback);
}

VideoFeedWidget::~VideoFeedWidget()
{
    m_inferenceTimer.stop();
    m_blinkTimer.stop();
    m_fallbackTimer.stop();

    if (m_dshowCapture) {
        m_dshowCapture->stop();
    }
    if (m_player) {
        m_player->stop();
    }
    if (m_camera) {
        m_camera->stop();
    }
}

QSize VideoFeedWidget::sizeHint() const
{
    return QSize(640, 360);
}

void VideoFeedWidget::setEagleEyeClassifier(EagleEyeClassifier *classifier)
{
    m_classifier = classifier;
}

bool VideoFeedWidget::initCamera(const QString &preferredDeviceName)
{
    if (m_player) {
        m_player->stop();
    }
    m_feedMode = FeedMode::LiveCamera;
    m_captureSession->setVideoSink(m_videoSink);

    // 1. DirectShow native capture for OBS Virtual Camera & Windows capture filters
    if (m_dshowCapture && m_dshowCapture->start(preferredDeviceName)) {
        m_cameraActive = true;
        m_fallbackTimer.stop();
        emit feedModeChanged(true);
        return true;
    }

    // 2. Fall back to Qt 6 QMediaDevices for WDM / Media Foundation cameras
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        emit statusMessage(QStringLiteral("No camera devices detected. Starting screen capture fallback..."));
        m_fallbackTimer.start(100);
        return false;
    }

    QCameraDevice selectedDevice = cameras.first();
    bool foundPreferred = false;

    for (const QCameraDevice &dev : cameras) {
        qInfo() << "VideoFeedWidget: Detected camera:" << dev.description();
        if (dev.description().contains(preferredDeviceName, Qt::CaseInsensitive)) {
            selectedDevice = dev;
            foundPreferred = true;
            break;
        }
    }

    emit statusMessage(QStringLiteral("Connecting to camera: %1").arg(selectedDevice.description()));

    if (m_camera) {
        m_camera->stop();
        delete m_camera;
    }

    m_camera = new QCamera(selectedDevice, this);
    m_captureSession->setCamera(m_camera);
    m_camera->start();

    m_cameraActive = true;
    m_fallbackTimer.stop();

    emit statusMessage(QStringLiteral("Camera active: %1 (Preferred: %2)")
                       .arg(selectedDevice.description())
                       .arg(foundPreferred ? QStringLiteral("Yes") : QStringLiteral("No (using default)")));
    emit feedModeChanged(true);
    return true;
}

bool VideoFeedWidget::openVideoFile(const QString &filePath)
{
    if (filePath.isEmpty() || !QFile::exists(filePath)) {
        emit statusMessage(QStringLiteral("Video file does not exist: %1").arg(filePath));
        return false;
    }

    if (m_camera) {
        m_camera->stop();
    }
    m_fallbackTimer.stop();
    m_cameraActive = false;
    m_feedMode = FeedMode::VideoFile;

    m_currentVideoPath = filePath;
    m_player->setVideoSink(m_videoSink);
    m_player->setSource(QUrl::fromLocalFile(filePath));
    m_player->setPlaybackRate(m_playbackRate);

    // Reset round tracking state for clean run analysis
    m_lastRound = 0;
    m_lastRoundStr = QStringLiteral("-");
    m_candidateRound = 0;
    m_candidateCount = 0;
    m_lastRoundStartPosMs = -1;
    m_lapTimer.restart();

    emit feedModeChanged(false);
    emit statusMessage(QStringLiteral("Loaded gameplay video: %1").arg(QFileInfo(filePath).fileName()));
    return true;
}

void VideoFeedWidget::play()
{
    if (m_feedMode == FeedMode::VideoFile && m_player) {
        m_player->play();
        emit statusMessage(QStringLiteral("Video playback started."));
    }
}

void VideoFeedWidget::pause()
{
    if (m_feedMode == FeedMode::VideoFile && m_player) {
        m_player->pause();
        emit statusMessage(QStringLiteral("Video playback paused."));
    }
}

void VideoFeedWidget::stop()
{
    if (m_feedMode == FeedMode::VideoFile && m_player) {
        m_player->stop();
        m_player->setPosition(0);
        m_lastRoundStartPosMs = 0;
        emit statusMessage(QStringLiteral("Video playback stopped."));
    }
}

void VideoFeedWidget::seek(qint64 positionMs)
{
    if (m_feedMode == FeedMode::VideoFile && m_player) {
        m_player->setPosition(positionMs);
    }
}

void VideoFeedWidget::setPlaybackRate(qreal rate)
{
    m_playbackRate = rate;
    if (m_player) {
        m_player->setPlaybackRate(rate);
    }
    // Dynamically adjust inference frequency based on playback speed
    int interval = std::max(60, static_cast<int>(350 / rate));
    m_inferenceTimer.setInterval(interval);

    emit statusMessage(QStringLiteral("Playback speed set to %1x").arg(rate, 0, 'f', 1));
}

bool VideoFeedWidget::isVideoPlaying() const
{
    return m_player && (m_player->playbackState() == QMediaPlayer::PlayingState);
}

bool VideoFeedWidget::isVideoFileLoaded() const
{
    return m_feedMode == FeedMode::VideoFile && !m_currentVideoPath.isEmpty();
}

void VideoFeedWidget::switchToLiveCamera()
{
    if (m_player) {
        m_player->stop();
    }
    initCamera();
}

void VideoFeedWidget::setStreamingActive(bool active)
{
    m_streamingActive = active;
    if (m_feedMode == FeedMode::LiveCamera) {
        if (!m_streamingActive) {
            if (m_dshowCapture) m_dshowCapture->stop();
            if (m_camera) m_camera->stop();
            m_fallbackTimer.stop();
            m_inferenceTimer.stop();
            emit statusMessage(QStringLiteral("Live Video Stream: PAUSED (Standby Mode)"));
        } else {
            initCamera();
            m_inferenceTimer.start(350);
            emit statusMessage(QStringLiteral("Live Video Stream: ACTIVE (Resumed)"));
        }
    } else {
        if (!m_streamingActive) {
            pause();
        } else {
            play();
        }
    }
    update();
}

void VideoFeedWidget::setTargetFps(int fps)
{
    if (m_dshowCapture) {
        m_dshowCapture->setTargetFps(fps);
    }
}

int VideoFeedWidget::targetFps() const
{
    return m_dshowCapture ? m_dshowCapture->targetFps() : 60;
}

void VideoFeedWidget::onDirectShowFrame(const QImage &image)
{
    if (m_feedMode != FeedMode::LiveCamera || !m_streamingActive || image.isNull()) return;

    {
        QMutexLocker locker(&m_frameMutex);
        m_currentFrame = image;
    }

    m_cameraActive = true;
    m_frameCount++;

    qint64 now = m_fpsTimer.elapsed();
    if (now >= 1000) {
        m_cameraFps = static_cast<int>(m_frameCount * 1000.0 / now);
        m_frameCount = 0;
        m_fpsTimer.restart();
        emit frameCaptured(m_cameraFps);
    }
    update();
}

void VideoFeedWidget::onVideoFrameChanged(const QVideoFrame &frame)
{
    if (!m_streamingActive || !frame.isValid()) return;

    QImage img = frame.toImage();
    if (img.isNull()) return;

    {
        QMutexLocker locker(&m_frameMutex);
        m_currentFrame = img;
    }

    m_frameCount++;
    if (m_fpsTimer.elapsed() >= 1000) {
        m_cameraFps = static_cast<int>(m_frameCount * 1000.0 / m_fpsTimer.elapsed());
        m_frameCount = 0;
        m_fpsTimer.restart();
        emit frameCaptured(m_cameraFps);
    }

    update();
}

void VideoFeedWidget::onScreenCaptureFallback()
{
    if (!m_streamingActive) return;
    if (m_cameraActive && !m_currentFrame.isNull()) return;

    // Grab primary screen as a fallback
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QPixmap pix = screen->grabWindow(0);
        if (!pix.isNull()) {
            QMutexLocker locker(&m_frameMutex);
            m_currentFrame = pix.toImage();
        }
        update();
    }
}

void VideoFeedWidget::onInferenceTimer()
{
    if (!m_streamingActive || !m_classifier || !m_classifier->isLoaded()) return;

    QImage frameCopy;
    {
        QMutexLocker locker(&m_frameMutex);
        if (m_currentFrame.isNull()) return;
        frameCopy = m_currentFrame;
    }

    int fw = frameCopy.width();
    int fh = frameCopy.height();
    if (fw <= 0 || fh <= 0) return;

    // Scale ROI according to current frame dimensions (defaults based on 1920x1080)
    double sx = fw / 1920.0;
    double sy = fh / 1080.0;

    int rx = static_cast<int>(m_roiX * sx);
    int ry = static_cast<int>(m_roiY * sy);
    int rw = static_cast<int>(m_roiW * sx);
    int rh = static_cast<int>(m_roiH * sy);

    rx = std::max(0, std::min(rx, fw - 10));
    ry = std::max(0, std::min(ry, fh - 10));
    rw = std::max(10, std::min(rw, fw - rx));
    rh = std::max(10, std::min(rh, fh - ry));

    QImage roiImg = frameCopy.copy(rx, ry, rw, rh);
    if (roiImg.isNull()) return;

    int detectedRound = 0;
    float confidence = 0.0f;
    QString roundStr = m_classifier->predictRound(roiImg, &detectedRound, &confidence, m_digitMode);

    // Ignore Rounds 1-5 (Roman numerals); Round 6 is the lowest valid round number
    if (roundStr.isEmpty() || detectedRound < 6 || confidence < 0.60f) {
        m_candidateCount = 0;
        m_candidateRound = 0;
        return;
    }

    m_lastConfidence = confidence;

    // 1. Initial round detection when starting a video or live stream
    if (m_lastRound == 0) {
        if (detectedRound == m_candidateRound) {
            m_candidateCount++;
        } else {
            m_candidateRound = detectedRound;
            m_candidateRoundStr = roundStr;
            m_candidateCount = 1;
        }

        if (m_candidateCount >= 2) {
            m_lastRound = detectedRound;
            m_lastRoundStr = roundStr;
            m_candidateCount = 0;
            if (m_feedMode == FeedMode::VideoFile && m_player) {
                m_lastRoundStartPosMs = m_player->position();
            } else {
                m_lastRoundStartPosMs = 0;
            }
            m_lapTimer.restart();

            emit statusMessage(QStringLiteral("EagleEye AI: Tracking initiated at Round %1").arg(detectedRound));
            emit lapProgressChanged(detectedRound, 0.0);
            update();
        }
        return;
    }

    // 2. Subsequent round detection: round change means previous round finished!
    if (detectedRound != m_lastRound) {
        if (detectedRound == m_candidateRound) {
            m_candidateCount++;
        } else {
            m_candidateRound = detectedRound;
            m_candidateRoundStr = roundStr;
            m_candidateCount = 1;
        }

        const int requiredMatches = (detectedRound == m_lastRound + 1) ? 3 : 10;
        if (m_candidateCount >= requiredMatches) {
            double durationSec = 0.0;
            if (m_feedMode == FeedMode::VideoFile && m_player) {
                qint64 curPos = m_player->position();
                if (m_lastRoundStartPosMs >= 0 && curPos >= m_lastRoundStartPosMs) {
                    durationSec = (curPos - m_lastRoundStartPosMs) / 1000.0;
                } else {
                    durationSec = m_lapTimer.restart() / 1000.0;
                }
                m_lastRoundStartPosMs = curPos;
            } else {
                durationSec = m_lapTimer.restart() / 1000.0;
            }

            int finishedRound = m_lastRound;
            QString finishedStr = m_lastRoundStr;

            m_lastRound = detectedRound;
            m_lastRoundStr = roundStr;
            m_candidateCount = 0;

            emit roundDetected(finishedRound, finishedStr, durationSec);
            emit lapProgressChanged(detectedRound, 0.0);
            update();
        }
    } else {
        m_lastRoundStr = roundStr;
        m_candidateCount = 0;
        emit lapProgressChanged(m_lastRound, currentLapElapsedSec());
        update();
    }
}

double VideoFeedWidget::currentLapElapsedSec() const
{
    if (m_feedMode == FeedMode::VideoFile && m_player) {
        qint64 pos = m_player->position();
        if (m_lastRoundStartPosMs >= 0 && pos >= m_lastRoundStartPosMs) {
            return (pos - m_lastRoundStartPosMs) / 1000.0;
        }
        return pos / 1000.0;
    }
    return m_lapTimer.elapsed() / 1000.0;
}

void VideoFeedWidget::setRoi(int x, int y, int w, int h)
{
    if (m_roiX != x || m_roiY != y || m_roiW != w || m_roiH != h) {
        m_roiX = x;
        m_roiY = y;
        m_roiW = w;
        m_roiH = h;
        saveSettings();
        emit roiChanged(QRect(m_roiX, m_roiY, m_roiW, m_roiH));
        update();
    }
}

QRect VideoFeedWidget::roi() const
{
    return QRect(m_roiX, m_roiY, m_roiW, m_roiH);
}

void VideoFeedWidget::setDigitMode(DigitMode mode)
{
    if (m_digitMode != mode) {
        m_digitMode = mode;
        saveSettings();
        emit digitModeChanged(mode);
        update();
    }
}

void VideoFeedWidget::setRoiPreset(DigitMode mode)
{
    switch (mode) {
    case DigitMode::OneDigit:
        setRoi(1808, 29, 65, 130);
        break;
    case DigitMode::TwoDigits:
        setRoi(1750, 29, 123, 130);
        break;
    case DigitMode::ThreeDigits:
    case DigitMode::Auto:
        setRoi(1684, 29, 189, 130);
        break;
    }
}

void VideoFeedWidget::resetRoiToDefault()
{
    setRoi(1684, 29, 189, 130);
}

void VideoFeedWidget::loadSettings()
{
    QSettings settings(QStringLiteral("CB26"), QStringLiteral("VideoFeed"));
    m_roiX = settings.value(QStringLiteral("roiX"), 1684).toInt();
    m_roiY = settings.value(QStringLiteral("roiY"), 29).toInt();
    m_roiW = settings.value(QStringLiteral("roiW"), 189).toInt();
    m_roiH = settings.value(QStringLiteral("roiH"), 130).toInt();
    int modeVal = settings.value(QStringLiteral("digitMode"), 0).toInt();
    if (modeVal >= 0 && modeVal <= 3) {
        m_digitMode = static_cast<DigitMode>(modeVal);
    }
}

void VideoFeedWidget::saveSettings()
{
    QSettings settings(QStringLiteral("CB26"), QStringLiteral("VideoFeed"));
    settings.setValue(QStringLiteral("roiX"), m_roiX);
    settings.setValue(QStringLiteral("roiY"), m_roiY);
    settings.setValue(QStringLiteral("roiW"), m_roiW);
    settings.setValue(QStringLiteral("roiH"), m_roiH);
    settings.setValue(QStringLiteral("digitMode"), static_cast<int>(m_digitMode));
}

QRect VideoFeedWidget::calculateTargetRect() const
{
    int w = width();
    int h = height();
    if (w <= 0 || h <= 0) return QRect();

    double frameAspect = 16.0 / 9.0;
    {
        QMutexLocker locker(const_cast<QMutex*>(&m_frameMutex));
        if (!m_currentFrame.isNull() && m_currentFrame.height() > 0) {
            frameAspect = static_cast<double>(m_currentFrame.width()) / m_currentFrame.height();
        }
    }

    double widgetAspect = static_cast<double>(w) / h;
    if (widgetAspect > frameAspect) {
        int th = h;
        int tw = static_cast<int>(th * frameAspect);
        int tx = (w - tw) / 2;
        return QRect(tx, 0, tw, th);
    } else {
        int tw = w;
        int th = static_cast<int>(tw / frameAspect);
        int ty = (h - th) / 2;
        return QRect(0, ty, tw, th);
    }
}

QRect VideoFeedWidget::roiScreenRect() const
{
    QRect targetRect = calculateTargetRect();
    if (targetRect.isEmpty()) return QRect();

    double scaleX = targetRect.width() / 1920.0;
    double scaleY = targetRect.height() / 1080.0;

    int rx = targetRect.x() + static_cast<int>(m_roiX * scaleX);
    int ry = targetRect.y() + static_cast<int>(m_roiY * scaleY);
    int rw = static_cast<int>(m_roiW * scaleX);
    int rh = static_cast<int>(m_roiH * scaleY);

    return QRect(rx, ry, rw, rh);
}

VideoFeedWidget::DragHandle VideoFeedWidget::hitTest(const QPoint &pos) const
{
    if (!m_showOverlay) return DragHandle::None;
    QRect r = roiScreenRect();
    if (r.isEmpty()) return DragHandle::None;

    const int hs = 10; // hit test tolerance
    // Check corners first
    if (QRect(r.left() - hs/2, r.top() - hs/2, hs, hs).contains(pos))
        return DragHandle::TopLeft;
    if (QRect(r.right() - hs/2, r.top() - hs/2, hs, hs).contains(pos))
        return DragHandle::TopRight;
    if (QRect(r.left() - hs/2, r.bottom() - hs/2, hs, hs).contains(pos))
        return DragHandle::BottomLeft;
    if (QRect(r.right() - hs/2, r.bottom() - hs/2, hs, hs).contains(pos))
        return DragHandle::BottomRight;

    // Check edges
    if (std::abs(pos.y() - r.top()) <= hs/2 && pos.x() >= r.left() && pos.x() <= r.right())
        return DragHandle::Top;
    if (std::abs(pos.y() - r.bottom()) <= hs/2 && pos.x() >= r.left() && pos.x() <= r.right())
        return DragHandle::Bottom;
    if (std::abs(pos.x() - r.left()) <= hs/2 && pos.y() >= r.top() && pos.y() <= r.bottom())
        return DragHandle::Left;
    if (std::abs(pos.x() - r.right()) <= hs/2 && pos.y() >= r.top() && pos.y() <= r.bottom())
        return DragHandle::Right;

    // Check inside
    if (r.contains(pos))
        return DragHandle::Move;

    return DragHandle::None;
}

void VideoFeedWidget::updateCursorForHandle(DragHandle handle)
{
    switch (handle) {
    case DragHandle::TopLeft:
    case DragHandle::BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case DragHandle::TopRight:
    case DragHandle::BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case DragHandle::Top:
    case DragHandle::Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case DragHandle::Left:
    case DragHandle::Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case DragHandle::Move:
        setCursor(Qt::SizeAllCursor);
        break;
    default:
        unsetCursor();
        break;
    }
}

void VideoFeedWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_showOverlay) {
        // Check if user clicked on any of the on-screen HUD pill buttons
        if (m_pillAutoRect.contains(event->pos())) {
            setDigitMode(DigitMode::Auto);
            event->accept();
            return;
        }
        if (m_pill1DRect.contains(event->pos())) {
            setDigitMode(DigitMode::OneDigit);
            event->accept();
            return;
        }
        if (m_pill2DRect.contains(event->pos())) {
            setDigitMode(DigitMode::TwoDigits);
            event->accept();
            return;
        }
        if (m_pill3DRect.contains(event->pos())) {
            setDigitMode(DigitMode::ThreeDigits);
            event->accept();
            return;
        }

        DragHandle handle = hitTest(event->pos());
        if (handle != DragHandle::None) {
            m_dragHandle = handle;
            m_dragStartMousePos = event->pos();
            m_dragStartRoi = QRect(m_roiX, m_roiY, m_roiW, m_roiH);
            updateCursorForHandle(handle);
            update();
            event->accept();
            return;
        }
    }
    QWidget::mousePressEvent(event);
}

void VideoFeedWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragHandle != DragHandle::None) {
        QRect targetRect = calculateTargetRect();
        if (targetRect.isEmpty()) return;

        double scaleX = targetRect.width() / 1920.0;
        double scaleY = targetRect.height() / 1080.0;
        if (scaleX <= 0 || scaleY <= 0) return;

        int dx = static_cast<int>((event->pos().x() - m_dragStartMousePos.x()) / scaleX);
        int dy = static_cast<int>((event->pos().y() - m_dragStartMousePos.y()) / scaleY);

        int x = m_dragStartRoi.x();
        int y = m_dragStartRoi.y();
        int w = m_dragStartRoi.width();
        int h = m_dragStartRoi.height();

        const int minW = 20;
        const int minH = 20;

        switch (m_dragHandle) {
        case DragHandle::Move:
            x += dx;
            y += dy;
            break;
        case DragHandle::TopLeft:
            x += dx;
            w -= dx;
            y += dy;
            h -= dy;
            break;
        case DragHandle::Top:
            y += dy;
            h -= dy;
            break;
        case DragHandle::TopRight:
            w += dx;
            y += dy;
            h -= dy;
            break;
        case DragHandle::Right:
            w += dx;
            break;
        case DragHandle::BottomRight:
            w += dx;
            h += dy;
            break;
        case DragHandle::Bottom:
            h += dy;
            break;
        case DragHandle::BottomLeft:
            x += dx;
            w -= dx;
            h += dy;
            break;
        case DragHandle::Left:
            x += dx;
            w -= dx;
            break;
        default:
            break;
        }

        if (w < minW) {
            if (m_dragHandle == DragHandle::Left || m_dragHandle == DragHandle::TopLeft || m_dragHandle == DragHandle::BottomLeft) {
                x -= (minW - w);
            }
            w = minW;
        }
        if (h < minH) {
            if (m_dragHandle == DragHandle::Top || m_dragHandle == DragHandle::TopLeft || m_dragHandle == DragHandle::TopRight) {
                y -= (minH - h);
            }
            h = minH;
        }

        // Clamp to 1920x1080 boundaries
        x = std::max(0, std::min(x, 1920 - w));
        y = std::max(0, std::min(y, 1080 - h));
        w = std::max(minW, std::min(w, 1920 - x));
        h = std::max(minH, std::min(h, 1080 - y));

        setRoi(x, y, w, h);
        event->accept();
        return;
    }

    if (m_showOverlay) {
        // If hovering over pills, show pointing hand cursor
        if (m_pillAutoRect.contains(event->pos()) ||
            m_pill1DRect.contains(event->pos()) ||
            m_pill2DRect.contains(event->pos()) ||
            m_pill3DRect.contains(event->pos())) {
            setCursor(Qt::PointingHandCursor);
        } else {
            DragHandle handle = hitTest(event->pos());
            if (handle != m_hoverHandle) {
                m_hoverHandle = handle;
                update();
            }
            updateCursorForHandle(handle);
        }
    } else {
        unsetCursor();
    }
    QWidget::mouseMoveEvent(event);
}

void VideoFeedWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_dragHandle != DragHandle::None) {
        m_dragHandle = DragHandle::None;
        DragHandle handle = hitTest(event->pos());
        updateCursorForHandle(handle);
        saveSettings();
        update();
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void VideoFeedWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void VideoFeedWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    int w = width();
    int h = height();

    // High-tech dark telemetry canvas
    p.fillRect(rect(), QColor(7, 10, 20));

    // If stream is paused by user toggle:
    if (!m_streamingActive) {
        int cx = w / 2;
        int cy = h / 2;

        QPen ringPen(QColor(30, 41, 59), 1, Qt::DotLine);
        p.setPen(ringPen);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPoint(cx, cy), 50, 50);
        p.drawEllipse(QPoint(cx, cy), 100, 100);
        p.drawEllipse(QPoint(cx, cy), 160, 160);

        QPen crossPen(QColor(51, 65, 85), 1, Qt::SolidLine);
        p.setPen(crossPen);
        p.drawLine(cx - 180, cy, cx + 180, cy);
        p.drawLine(cx, cy - 120, cx, cy + 120);

        QRect pillRect(cx - 120, cy - 35, 240, 32);
        p.setPen(QColor(245, 158, 11)); // Amber
        p.setBrush(QColor(15, 23, 42, 230));
        p.drawRoundedRect(pillRect, 6, 6);

        QFont font = p.font();
        font.setPointSize(10);
        font.setBold(true);
        p.setFont(font);
        p.setPen(QColor(245, 158, 11));
        p.drawText(pillRect, Qt::AlignCenter, QStringLiteral("● LIVE STREAM PAUSED"));

        font.setPointSize(9);
        font.setBold(false);
        p.setFont(font);
        p.setPen(QColor(148, 163, 184));
        p.drawText(QRect(cx - 200, cy + 8, 400, 40), Qt::AlignCenter, 
                   QStringLiteral("EagleEye AI Camera Acquisition Idle\nClick 'Live Stream' Toggle to Resume"));
        return;
    }

    QImage frame;
    {
        QMutexLocker locker(&m_frameMutex);
        frame = m_currentFrame;
    }

    if (frame.isNull()) {
        int cx = w / 2;
        int cy = h / 2;

        QPen ringPen(QColor(30, 41, 59), 1, Qt::DashLine);
        p.setPen(ringPen);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPoint(cx, cy), 60, 60);
        p.drawEllipse(QPoint(cx, cy), 130, 130);

        QFont font = p.font();
        font.setPointSize(10);
        font.setBold(true);
        p.setFont(font);
        p.setPen(QColor(56, 189, 248)); // Cyan

        if (m_feedMode == FeedMode::VideoFile) {
            p.drawText(QRect(cx - 220, cy - 25, 440, 24), Qt::AlignCenter, 
                       QStringLiteral("GAMEPLAY VIDEO READY"));
            font.setPointSize(8);
            font.setBold(false);
            p.setFont(font);
            p.setPen(QColor(148, 163, 184));
            p.drawText(QRect(cx - 220, cy + 5, 440, 20), Qt::AlignCenter, 
                       QStringLiteral("Click Play or Open Video to Begin EagleEye AI Telemetry Run"));
        } else {
            p.drawText(QRect(cx - 220, cy - 25, 440, 24), Qt::AlignCenter, 
                       QStringLiteral("SEARCHING FOR OBS VIRTUAL CAMERA SIGNAL..."));
            font.setPointSize(8);
            font.setBold(false);
            p.setFont(font);
            p.setPen(QColor(148, 163, 184));
            p.drawText(QRect(cx - 220, cy + 5, 440, 20), Qt::AlignCenter, 
                       QStringLiteral("1920 x 1080 Real-Time Stream Target • EagleEye AI Standby"));
        }
        return;
    }

    // Preserve 16:9 aspect ratio
    double frameAspect = static_cast<double>(frame.width()) / frame.height();
    double widgetAspect = static_cast<double>(w) / h;

    QRect targetRect;
    if (widgetAspect > frameAspect) {
        int th = h;
        int tw = static_cast<int>(th * frameAspect);
        int tx = (w - tw) / 2;
        targetRect = QRect(tx, 0, tw, th);
    } else {
        int tw = w;
        int th = static_cast<int>(tw / frameAspect);
        int ty = (h - th) / 2;
        targetRect = QRect(0, ty, tw, th);
    }

    // Draw the video frame
    p.drawImage(targetRect, frame);

    if (m_showOverlay) {
        // Calculate ROI mapped to targetRect
        double scaleX = targetRect.width() / 1920.0;
        double scaleY = targetRect.height() / 1080.0;

        int rx = targetRect.x() + static_cast<int>(m_roiX * scaleX);
        int ry = targetRect.y() + static_cast<int>(m_roiY * scaleY);
        int rw = static_cast<int>(m_roiW * scaleX);
        int rh = static_cast<int>(m_roiH * scaleY);

        QRect roiScreenRect(rx, ry, rw, rh);

        // Subtle tactical glow/fill when hovering or dragging
        if (m_dragHandle != DragHandle::None || m_hoverHandle != DragHandle::None) {
            p.fillRect(roiScreenRect, QColor(0, 255, 68, 25));
        }

        // Draw green bounding box around round number
        QPen greenPen(QColor(0, 255, 68), 2, Qt::SolidLine);
        p.setPen(greenPen);
        p.setBrush(Qt::NoBrush);
        p.drawRect(roiScreenRect);

        // Draw corner brackets on ROI for tactical HUD look
        int cornerLen = std::max(4, rw / 6);
        p.drawLine(rx, ry, rx + cornerLen, ry);
        p.drawLine(rx, ry, rx, ry + cornerLen);
        p.drawLine(rx + rw, ry, rx + rw - cornerLen, ry);
        p.drawLine(rx + rw, ry, rx + rw, ry + cornerLen);
        p.drawLine(rx, ry + rh, rx + cornerLen, ry + rh);
        p.drawLine(rx, ry + rh, rx, ry + rh - cornerLen);
        p.drawLine(rx + rw, ry + rh, rx + rw - cornerLen, ry + rh);
        p.drawLine(rx + rw, ry + rh, rx + rw, ry + rh - cornerLen);

        // Draw 8 interactive resize handles
        const int hs = 6;
        auto drawHandle = [&](int hx, int hy, DragHandle handleType) {
            QRect hr(hx - hs / 2, hy - hs / 2, hs, hs);
            bool isHighlight = (m_dragHandle == handleType || m_hoverHandle == handleType);
            p.fillRect(hr, isHighlight ? QColor(255, 255, 255) : QColor(0, 255, 68));
            p.setPen(QColor(0, 40, 10));
            p.drawRect(hr);
        };
        drawHandle(rx, ry, DragHandle::TopLeft);
        drawHandle(rx + rw / 2, ry, DragHandle::Top);
        drawHandle(rx + rw, ry, DragHandle::TopRight);
        drawHandle(rx + rw, ry + rh / 2, DragHandle::Right);
        drawHandle(rx + rw, ry + rh, DragHandle::BottomRight);
        drawHandle(rx + rw / 2, ry + rh, DragHandle::Bottom);
        drawHandle(rx, ry + rh, DragHandle::BottomLeft);
        drawHandle(rx, ry + rh / 2, DragHandle::Left);

        // Draw dashed vertical digit split guides inside ROI
        double aspect = rw / static_cast<double>(std::max(1, rh));
        QPen guidePen(QColor(0, 255, 68, 140), 1, Qt::DashLine);
        p.setPen(guidePen);
        if (m_digitMode == DigitMode::TwoDigits || (m_digitMode == DigitMode::Auto && aspect >= 0.65 && aspect < 1.15)) {
            p.drawLine(rx + rw / 2, ry, rx + rw / 2, ry + rh);
        } else if (m_digitMode == DigitMode::ThreeDigits || (m_digitMode == DigitMode::Auto && aspect >= 1.15)) {
            int x1 = rx + static_cast<int>(rw * 0.35);
            int x2 = rx + static_cast<int>(rw * 0.65);
            p.drawLine(x1, ry, x1, ry + rh);
            p.drawLine(x2, ry, x2, ry + rh);
        }

        // Label above ROI: EagleEye AI Round Tag
        QFont font = p.font();
        font.setPointSize(8);
        font.setBold(true);
        p.setFont(font);

        QString modeStr;
        switch (m_digitMode) {
        case DigitMode::OneDigit: modeStr = QStringLiteral("1-DIGIT"); break;
        case DigitMode::TwoDigits: modeStr = QStringLiteral("2-DIGIT"); break;
        case DigitMode::ThreeDigits: modeStr = QStringLiteral("3-DIGIT"); break;
        case DigitMode::Auto: modeStr = QStringLiteral("AUTO"); break;
        }

        QString tag = (m_dragHandle != DragHandle::None)
            ? QStringLiteral("ROI: %1,%2 [%3x%4] • %5").arg(m_roiX).arg(m_roiY).arg(m_roiW).arg(m_roiH).arg(modeStr)
            : (m_lastRound > 0)
                ? QStringLiteral("ROUND: %1 (%2%) • %3").arg(m_lastRoundStr).arg(static_cast<int>(m_lastConfidence * 100)).arg(modeStr)
                : QStringLiteral("ROUND: SCANNING... • %1").arg(modeStr);

        int tagW = std::max(rw, 150);
        int tagY = ry - 18;
        if (tagY < targetRect.top() + 26) {
            tagY = ry + rh + 4;
        }
        QRect tagRect(rx, tagY, tagW, 16);
        p.fillRect(tagRect, QColor(0, 0, 0, 190));
        p.setPen(QColor(0, 255, 68));
        p.drawText(tagRect, Qt::AlignCenter, tag);

        // Interactive On-Screen Mode Selector Pills: [AUTO] [1D] [2D] [3D]
        int pillY = (tagY == ry - 18) ? (tagY - 18) : (tagY + 18);
        if (pillY < targetRect.top() + 26) {
            pillY = tagRect.bottom() + 2;
        }
        int pillH = 15;
        int curX = rx;

        m_pillAutoRect = QRect(curX, pillY, 36, pillH); curX += 38;
        m_pill1DRect   = QRect(curX, pillY, 24, pillH); curX += 26;
        m_pill2DRect   = QRect(curX, pillY, 24, pillH); curX += 26;
        m_pill3DRect   = QRect(curX, pillY, 24, pillH);

        auto drawPill = [&](const QRect &pr, const QString &text, bool active) {
            p.setPen(active ? QColor(0, 255, 68) : QColor(51, 65, 85));
            p.setBrush(active ? QColor(0, 255, 68, 220) : QColor(15, 23, 42, 210));
            p.drawRoundedRect(pr, 3, 3);
            p.setPen(active ? QColor(0, 0, 0) : QColor(148, 163, 184));
            QFont pf = p.font();
            pf.setPointSize(7);
            pf.setBold(true);
            p.setFont(pf);
            p.drawText(pr, Qt::AlignCenter, text);
        };

        drawPill(m_pillAutoRect, QStringLiteral("AUTO"), m_digitMode == DigitMode::Auto);
        drawPill(m_pill1DRect, QStringLiteral("1D"), m_digitMode == DigitMode::OneDigit);
        drawPill(m_pill2DRect, QStringLiteral("2D"), m_digitMode == DigitMode::TwoDigits);
        drawPill(m_pill3DRect, QStringLiteral("3D"), m_digitMode == DigitMode::ThreeDigits);

        // Top Status HUD Bar
        QRect hudRect(targetRect.x(), targetRect.y(), targetRect.width(), 26);
        p.fillRect(hudRect, QColor(0, 0, 0, 160));

        if (m_feedMode == FeedMode::LiveCamera) {
            // LIVE Indicator (blinking red circle)
            if (m_liveBlinkState) {
                p.setBrush(QColor(255, 30, 30));
                p.setPen(Qt::NoPen);
                p.drawEllipse(targetRect.x() + 8, targetRect.y() + 8, 10, 10);
            }
            p.setPen(Qt::white);
            font.setPointSize(8);
            font.setBold(true);
            p.setFont(font);
            p.drawText(QRect(targetRect.x() + 24, targetRect.y() + 5, 45, 16), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("LIVE"));

            // Resolution & FPS
            font.setBold(false);
            p.setFont(font);
            p.setPen(QColor(220, 220, 220));
            QString streamInfo = QStringLiteral("OBS Virtual Camera | 1920x1080 | %1 FPS").arg(m_cameraFps > 0 ? m_cameraFps : 60);
            p.drawText(QRect(targetRect.x() + 75, targetRect.y() + 5, 280, 16), Qt::AlignLeft | Qt::AlignVCenter, streamInfo);
        } else {
            // Video File Mode HUD
            bool isPlaying = isVideoPlaying();
            p.setBrush(isPlaying ? QColor(0, 230, 80) : (m_player && m_player->playbackState() == QMediaPlayer::PausedState ? QColor(255, 170, 0) : QColor(160, 160, 160)));
            p.setPen(Qt::NoPen);
            p.drawEllipse(targetRect.x() + 8, targetRect.y() + 8, 10, 10);

            p.setPen(Qt::white);
            font.setPointSize(8);
            font.setBold(true);
            p.setFont(font);
            QString stateStr = isPlaying ? QStringLiteral("PLAYING") : (m_player && m_player->playbackState() == QMediaPlayer::PausedState ? QStringLiteral("PAUSED") : QStringLiteral("STOPPED"));
            p.drawText(QRect(targetRect.x() + 24, targetRect.y() + 5, 55, 16), Qt::AlignLeft | Qt::AlignVCenter, stateStr);

            // Time & File Info
            font.setBold(false);
            p.setFont(font);
            p.setPen(QColor(220, 220, 220));

            int curSec = static_cast<int>(m_videoPositionMs / 1000);
            int totSec = static_cast<int>(m_videoDurationMs / 1000);
            QString timeStr = QString::asprintf("%02d:%02d / %02d:%02d", curSec / 60, curSec % 60, totSec / 60, totSec % 60);
            QString videoName = QFileInfo(m_currentVideoPath).fileName();
            if (videoName.length() > 22) {
                videoName = videoName.left(19) + QStringLiteral("...");
            }
            QString fileInfo = QStringLiteral("%1 | %2 | %3x (%4 FPS)")
                                   .arg(videoName, timeStr)
                                   .arg(m_playbackRate, 0, 'f', 1)
                                   .arg(m_cameraFps > 0 ? m_cameraFps : 60);
            p.drawText(QRect(targetRect.x() + 85, targetRect.y() + 5, 340, 16), Qt::AlignLeft | Qt::AlignVCenter, fileInfo);
        }

        // Current Lap Timer in top right of HUD
        int elapsedSec = static_cast<int>(currentLapElapsedSec());
        int mm = elapsedSec / 60;
        int ss = elapsedSec % 60;
        QString lapTimerStr = QString::asprintf("LAP TIME: %02d:%02d", mm, ss);
        p.setPen(QColor(255, 204, 0));
        p.drawText(QRect(targetRect.right() - 140, targetRect.y() + 5, 130, 16), Qt::AlignRight | Qt::AlignVCenter, lapTimerStr);
    }
}

void VideoFeedWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *toggleOverlay = menu.addAction(m_showOverlay ? tr("Hide AI Overlay") : tr("Show AI Overlay"));
    connect(toggleOverlay, &QAction::triggered, this, [this]() {
        setOverlayEnabled(!m_showOverlay);
    });

    // Digit Detection Mode selection
    QMenu *modeMenu = menu.addMenu(tr("Digit Detection Mode"));
    
    QAction *actAuto = modeMenu->addAction(tr("Auto Detection (Rounds 6-999)"));
    actAuto->setCheckable(true);
    actAuto->setChecked(m_digitMode == DigitMode::Auto);
    connect(actAuto, &QAction::triggered, this, [this]() { setDigitMode(DigitMode::Auto); });

    QAction *act1D = modeMenu->addAction(tr("1 Digit Detection (Rounds 6-9)"));
    act1D->setCheckable(true);
    act1D->setChecked(m_digitMode == DigitMode::OneDigit);
    connect(act1D, &QAction::triggered, this, [this]() { setDigitMode(DigitMode::OneDigit); });

    QAction *act2D = modeMenu->addAction(tr("2 Digits Detection (Rounds 10-99)"));
    act2D->setCheckable(true);
    act2D->setChecked(m_digitMode == DigitMode::TwoDigits);
    connect(act2D, &QAction::triggered, this, [this]() { setDigitMode(DigitMode::TwoDigits); });

    QAction *act3D = modeMenu->addAction(tr("3 Digits Detection (Rounds 100+)"));
    act3D->setCheckable(true);
    act3D->setChecked(m_digitMode == DigitMode::ThreeDigits);
    connect(act3D, &QAction::triggered, this, [this]() { setDigitMode(DigitMode::ThreeDigits); });

    // Quick presets to snap crop box size and position
    QMenu *presetMenu = menu.addMenu(tr("Crop Area Presets"));
    QAction *pre1D = presetMenu->addAction(tr("Snap to 1 Digit Size (65 x 130)"));
    connect(pre1D, &QAction::triggered, this, [this]() {
        setRoiPreset(DigitMode::OneDigit);
        setDigitMode(DigitMode::OneDigit);
    });

    QAction *pre2D = presetMenu->addAction(tr("Snap to 2 Digits Size (123 x 130)"));
    connect(pre2D, &QAction::triggered, this, [this]() {
        setRoiPreset(DigitMode::TwoDigits);
        setDigitMode(DigitMode::TwoDigits);
    });

    QAction *pre3D = presetMenu->addAction(tr("Snap to 3 Digits Size (189 x 130)"));
    connect(pre3D, &QAction::triggered, this, [this]() {
        setRoiPreset(DigitMode::ThreeDigits);
        setDigitMode(DigitMode::ThreeDigits);
    });

    presetMenu->addSeparator();
    QAction *actResetRoi = presetMenu->addAction(tr("Reset Crop Area to Default"));
    connect(actResetRoi, &QAction::triggered, this, &VideoFeedWidget::resetRoiToDefault);

    QMenu *fpsMenu = menu.addMenu(tr("Stream Framerate (FPS)"));
    const int fpsOptions[] = { 60, 30, 15 };
    for (int fps : fpsOptions) {
        QString label = (fps == 60) ? QStringLiteral("60 FPS (Ultra Smooth)")
                      : (fps == 30) ? QStringLiteral("30 FPS (Balanced)")
                                    : QStringLiteral("15 FPS (Power Saver)");
        if (targetFps() == fps) {
            label += QStringLiteral("  ✓");
        }
        QAction *act = fpsMenu->addAction(label);
        connect(act, &QAction::triggered, this, [this, fps]() {
            setTargetFps(fps);
        });
    }

    menu.addSeparator();

    if (m_feedMode == FeedMode::VideoFile) {
        if (isVideoPlaying()) {
            QAction *actPause = menu.addAction(tr("Pause Playback"));
            connect(actPause, &QAction::triggered, this, &VideoFeedWidget::pause);
        } else {
            QAction *actPlay = menu.addAction(tr("Resume Playback"));
            connect(actPlay, &QAction::triggered, this, &VideoFeedWidget::play);
        }
        QAction *actStop = menu.addAction(tr("Stop Playback"));
        connect(actStop, &QAction::triggered, this, &VideoFeedWidget::stop);

        menu.addSeparator();
        QAction *actLive = menu.addAction(tr("Switch to OBS Virtual Camera (Live)"));
        connect(actLive, &QAction::triggered, this, &VideoFeedWidget::switchToLiveCamera);
    } else {
        QAction *reconnectCamera = menu.addAction(tr("Reconnect Camera"));
        connect(reconnectCamera, &QAction::triggered, this, [this]() {
            initCamera();
        });
    }

    menu.exec(event->globalPos());
}
