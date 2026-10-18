#ifndef DSHOW_CAPTURE_H
#define DSHOW_CAPTURE_H

#include <QObject>
#include <QImage>
#include <QString>

class DirectShowCaptureWorker;

class DirectShowCapture : public QObject
{
    Q_OBJECT

public:
    explicit DirectShowCapture(QObject *parent = nullptr);
    ~DirectShowCapture() override;

    bool start(const QString &preferredDeviceName = QStringLiteral("OBS Virtual Camera"));
    void stop();
    bool isRunning() const;

    void setTargetFps(int fps);
    int targetFps() const;

signals:
    void frameReady(const QImage &image);
    void statusMessage(const QString &msg);

private:
    DirectShowCaptureWorker *m_worker = nullptr;
};

#endif // DSHOW_CAPTURE_H
