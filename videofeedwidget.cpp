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

    m_videoSink = new QVideoSink(this);
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &VideoFeedWidget::onVideoFrameChanged);

    m_captureSession = new QMediaCaptureSession(this);
    m_captureSession->setVideoSink(m_videoSink);

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

    // AI Inference timer (run ~3 times per second at 1x)
    connect(&m_inferenceTimer, &QTimer::timeout, this, &VideoFeedWidget::onInferenceTimer);
    m_inferenceTimer.start(350);

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
    m_lastRoundStartPosMs = 0;
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

void VideoFeedWidget::onVideoFrameChanged(const QVideoFrame &frame)
{
    if (!frame.isValid()) return;

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
    if (!m_classifier || !m_classifier->isLoaded()) return;

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
    QString roundStr = m_classifier->predictRound(roiImg, &detectedRound, &confidence);

    if (roundStr.isEmpty() || detectedRound <= 0 || confidence < 0.35f) {
        m_candidateCount = 0;
        m_candidateRound = 0;
        return;
    }

    m_lastConfidence = confidence;

    // Trigger round detection with debouncing (requires 2 consecutive matching frames)
    if (detectedRound != m_lastRound) {
        if (detectedRound == m_candidateRound) {
            m_candidateCount++;
        } else {
            m_candidateRound = detectedRound;
            m_candidateRoundStr = roundStr;
            m_candidateCount = 1;
        }

        if (m_candidateCount >= 2) {
            double durationSec = 0.0;
            if (m_feedMode == FeedMode::VideoFile && m_player) {
                qint64 curPos = m_player->position();
                if (m_lastRoundStartPosMs > 0 && curPos >= m_lastRoundStartPosMs) {
                    durationSec = (curPos - m_lastRoundStartPosMs) / 1000.0;
                } else {
                    durationSec = m_lapTimer.restart() / 1000.0;
                }
                m_lastRoundStartPosMs = curPos;
            } else {
                durationSec = m_lapTimer.restart() / 1000.0;
            }

            m_lastRound = detectedRound;
            m_lastRoundStr = roundStr;
            m_candidateCount = 0;

            emit roundDetected(detectedRound, roundStr, durationSec);
            update();
        }
    } else {
        m_lastRoundStr = roundStr;
        m_candidateCount = 0;
        update();
    }
}

double VideoFeedWidget::currentLapElapsedSec() const
{
    if (m_feedMode == FeedMode::VideoFile && m_player) {
        qint64 pos = m_player->position();
        if (pos >= m_lastRoundStartPosMs && m_lastRoundStartPosMs > 0) {
            return (pos - m_lastRoundStartPosMs) / 1000.0;
        }
        return pos / 1000.0;
    }
    return m_lapTimer.elapsed() / 1000.0;
}

void VideoFeedWidget::setRoi(int x, int y, int w, int h)
{
    m_roiX = x;
    m_roiY = y;
    m_roiW = w;
    m_roiH = h;
    update();
}

QRect VideoFeedWidget::roi() const
{
    return QRect(m_roiX, m_roiY, m_roiW, m_roiH);
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

    // Background colour set to #bcbcbc
    p.fillRect(rect(), QColor(QStringLiteral("#bcbcbc")));

    QImage frame;
    {
        QMutexLocker locker(&m_frameMutex);
        frame = m_currentFrame;
    }

    if (frame.isNull()) {
        // Standby Screen
        p.setPen(Qt::black);
        QFont font = p.font();
        font.setPointSize(10);
        font.setBold(true);
        p.setFont(font);

        if (m_feedMode == FeedMode::VideoFile) {
            p.drawText(rect(), Qt::AlignCenter, 
                       QStringLiteral("Gameplay Video Ready\nClick Play or Open Video to begin analysis"));
        } else {
            p.drawText(rect(), Qt::AlignCenter, 
                       QStringLiteral("Waiting for OBS Virtual Camera Stream...\n(1920 x 1080 Delayed Game Live Stream)"));
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

        // Label above ROI: EagleEye AI Round Tag
        QFont font = p.font();
        font.setPointSize(8);
        font.setBold(true);
        p.setFont(font);

        QString tag = (m_lastRound > 0)
            ? QStringLiteral("ROUND: %1 (%2%)").arg(m_lastRoundStr).arg(static_cast<int>(m_lastConfidence * 100))
            : QStringLiteral("ROUND: SCANNING...");
        QRect tagRect(rx, std::max(targetRect.top(), ry - 18), rw, 16);
        p.fillRect(tagRect, QColor(0, 0, 0, 180));
        p.setPen(QColor(0, 255, 68));
        p.drawText(tagRect, Qt::AlignCenter, tag);

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
