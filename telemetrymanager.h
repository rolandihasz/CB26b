#ifndef TELEMETRYMANAGER_H
#define TELEMETRYMANAGER_H

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>

struct GpuStats {
    QString name;
    int temperatureC = 0;
    int fanSpeedPercent = 0;
    int gpuUtilPercent = 0;
    int memUsedMb = 0;
    int memTotalMb = 0;
};

class TelemetryManager : public QObject {
    Q_OBJECT

public:
    explicit TelemetryManager(QObject *parent = nullptr);
    ~TelemetryManager() override;

    void startMonitoring();
    void stopMonitoring();

    QString internalIp() const { return m_internalIp; }
    QString externalIp() const { return m_externalIp; }
    GpuStats lastGpuStats() const { return m_lastGpuStats; }

    QStringList screenDiagnostics() const;

signals:
    void logMessage(const QString &msg);
    void gpuUpdated(const GpuStats &stats);
    void ipUpdated(const QString &intIp, const QString &extIp);

public slots:
    void queryGpu();
    void resolveExternalIp();
    void resolveInternalIp();

private slots:
    void onGpuProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QTimer m_gpuTimer;
    QProcess *m_gpuProcess = nullptr;
    QNetworkAccessManager m_nam;

    QString m_internalIp;
    QString m_externalIp;
    GpuStats m_lastGpuStats;
};

#endif // TELEMETRYMANAGER_H
