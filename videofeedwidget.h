#ifndef VIDEOFEEDWIDGET_H
#define VIDEOFEEDWIDGET_H

#include <QWidget>
#include <QImage>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QVideoFrame>
#include <QTimer>
#include <QElapsedTimer>
#include <QMutex>
#include <QFileInfo>
#include "eagleeye_classifier.h"

class VideoFeedWidget : public QWidget {
    Q_OBJECT

public:
    enum class FeedMode {
        LiveCamera,
        VideoFile
    };

    explicit VideoFeedWidget(QWidget *parent = nullptr);
    ~VideoFeedWidget() override;

    QSize sizeHint() const override;

    bool initCamera(const QString &preferredDeviceName = QStringLiteral("OBS Virtual Camera"));
    void setEagleEyeClassifier(EagleEyeClassifier *classifier);

    // Video File Playback & Analysis API
    bool openVideoFile(const QString &filePath);
    void play();
    void pause();
    void stop();
    void seek(qint64 positionMs);
    void setPlaybackRate(qreal rate);
    qreal playbackRate() const { return m_playbackRate; }

    bool isVideoPlaying() const;
    bool isVideoFileLoaded() const;
    FeedMode feedMode() const { return m_feedMode; }
    bool isLiveCameraMode() const { return m_feedMode == FeedMode::LiveCamera; }
    void switchToLiveCamera();

    qint64 videoPosition() const { return m_videoPositionMs; }
    qint64 videoDuration() const { return m_videoDurationMs; }
    QString currentVideoPath() const { return m_currentVideoPath; }

    void setRoi(int x, int y, int w, int h);
    QRect roi() const;

    int currentRound() const { return m_lastRound; }
    double currentLapElapsedSec() const;

    void setOverlayEnabled(bool enabled) { m_showOverlay = enabled; update(); }
    bool isOverlayEnabled() const { return m_showOverlay; }

signals:
    void roundDetected(int roundNumber, const QString &roundStr, double durationSec);
    void statusMessage(const QString &msg);
    void frameCaptured(int fps);
    void videoPositionChanged(qint64 positionMs, qint64 durationMs);
    void videoDurationChanged(qint64 durationMs);
    void playbackStateChanged(bool isPlaying);
    void feedModeChanged(bool isLiveCamera);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onVideoFrameChanged(const QVideoFrame &frame);
    void onInferenceTimer();
    void onScreenCaptureFallback();

private:
    FeedMode m_feedMode = FeedMode::LiveCamera;

    QCamera *m_camera = nullptr;
    QMediaCaptureSession *m_captureSession = nullptr;
    QVideoSink *m_videoSink = nullptr;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QString m_currentVideoPath;
    qint64 m_videoPositionMs = 0;
    qint64 m_videoDurationMs = 0;
    qreal m_playbackRate = 1.0;
    qint64 m_lastRoundStartPosMs = 0;

    EagleEyeClassifier *m_classifier = nullptr;

    QImage m_currentFrame;
    QMutex m_frameMutex;

    // ROI coordinates based on 1920x1080 resolution
    int m_roiX = 1684;
    int m_roiY = 29;
    int m_roiW = 189;
    int m_roiH = 130;

    int m_lastRound = 0;
    QString m_lastRoundStr = QStringLiteral("-");
    float m_lastConfidence = 0.0f;

    // Debouncing buffer to prevent single-frame lighting/glitch false triggers
    int m_candidateRound = 0;
    QString m_candidateRoundStr;
    int m_candidateCount = 0;

    QElapsedTimer m_lapTimer;
    QElapsedTimer m_fpsTimer;
    int m_frameCount = 0;
    int m_cameraFps = 0;

    QTimer m_inferenceTimer;
    QTimer m_fallbackTimer;

    bool m_cameraActive = false;
    bool m_showOverlay = true;
    bool m_liveBlinkState = true;
    QTimer m_blinkTimer;
};

#endif // VIDEOFEEDWIDGET_H
