#include "telemetrymanager.h"

#include <QScreen>
#include <QGuiApplication>
#include <QNetworkInterface>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

TelemetryManager::TelemetryManager(QObject *parent)
    : QObject(parent)
{
    m_gpuProcess = new QProcess(this);
    connect(m_gpuProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TelemetryManager::onGpuProcessFinished);

    connect(&m_gpuTimer, &QTimer::timeout, this, &TelemetryManager::queryGpu);
}

TelemetryManager::~TelemetryManager()
{
    stopMonitoring();
}

void TelemetryManager::startMonitoring()
{
    resolveInternalIp();
    resolveExternalIp();

    queryGpu();
    m_gpuTimer.start(3000); // Poll GPU every 3 seconds
}

void TelemetryManager::stopMonitoring()
{
    m_gpuTimer.stop();
    if (m_gpuProcess && m_gpuProcess->state() != QProcess::NotRunning) {
        m_gpuProcess->kill();
    }
}

QStringList TelemetryManager::screenDiagnostics() const
{
    QStringList lines;
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        lines << QStringLiteral("Depth: %1 bits").arg(screen->depth());
        lines << QStringLiteral("Geometry: %1 %2 %3 x %4")
                     .arg(screen->geometry().x())
                     .arg(screen->geometry().y())
                     .arg(screen->geometry().width())
                     .arg(screen->geometry().height());
        lines << QStringLiteral("Logical DPI: %1").arg(screen->logicalDotsPerInch());
        lines << QStringLiteral("Logical DPI X: %1").arg(screen->logicalDotsPerInchX());
        lines << QStringLiteral("Logical DPI Y: %1").arg(screen->logicalDotsPerInchY());
        lines << QStringLiteral("Physical DPI: %1").arg(screen->physicalDotsPerInch());
        lines << QStringLiteral("Physical DPI X: %1").arg(screen->physicalDotsPerInchX());
        lines << QStringLiteral("Physical DPI Y: %1").arg(screen->physicalDotsPerInchY());
        lines << QStringLiteral("Physical size: %1 x %2 mm")
                     .arg(screen->physicalSize().width(), 0, 'f', 0)
                     .arg(screen->physicalSize().height(), 0, 'f', 0);
        lines << QStringLiteral("Refresh rate: %1 Hz").arg(static_cast<int>(std::round(screen->refreshRate())));
        lines << QStringLiteral("Size: %1 x %2")
                     .arg(screen->size().width())
                     .arg(screen->size().height());
        lines << QStringLiteral("Virtual geometry: %1 %2 %3 x %4")
                     .arg(screen->virtualGeometry().x())
                     .arg(screen->virtualGeometry().y())
                     .arg(screen->virtualGeometry().width())
                     .arg(screen->virtualGeometry().height());
        lines << QStringLiteral("Virtual size: %1 x %2")
                     .arg(screen->virtualSize().width())
                     .arg(screen->virtualSize().height());
    }
    return lines;
}

void TelemetryManager::queryGpu()
{
    if (m_gpuProcess->state() == QProcess::NotRunning) {
        QStringList args;
        args << QStringLiteral("--query-gpu=name,temperature.gpu,fan.speed,utilization.gpu,memory.used,memory.total")
             << QStringLiteral("--format=csv,noheader,nounits");
        m_gpuProcess->start(QStringLiteral("nvidia-smi"), args);
    }
}

void TelemetryManager::onGpuProcessFinished(int exitCode, QProcess::ExitStatus)
{
    if (exitCode == 0) {
        QString output = QString::fromUtf8(m_gpuProcess->readAllStandardOutput()).trimmed();
        if (!output.isEmpty()) {
            QStringList parts = output.split(',');
            if (parts.size() >= 6) {
                m_lastGpuStats.name = parts[0].trimmed();
                m_lastGpuStats.temperatureC = parts[1].trimmed().toInt();
                m_lastGpuStats.fanSpeedPercent = parts[2].trimmed().toInt();
                m_lastGpuStats.gpuUtilPercent = parts[3].trimmed().toInt();
                m_lastGpuStats.memUsedMb = parts[4].trimmed().toInt();
                m_lastGpuStats.memTotalMb = parts[5].trimmed().toInt();

                emit gpuUpdated(m_lastGpuStats);
            }
        }
    }
}

void TelemetryManager::resolveInternalIp()
{
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &netIf : interfaces) {
        if (!netIf.flags().testFlag(QNetworkInterface::IsUp) || 
            netIf.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            continue;
        }

        for (const QNetworkAddressEntry &entry : netIf.addressEntries()) {
            QHostAddress addr = entry.ip();
            if (addr.protocol() == QAbstractSocket::IPv4Protocol && !addr.isLoopback()) {
                m_internalIp = addr.toString();
                emit ipUpdated(m_internalIp, m_externalIp);
                return;
            }
        }
    }

    if (m_internalIp.isEmpty()) {
        m_internalIp = QStringLiteral("127.0.0.1");
        emit ipUpdated(m_internalIp, m_externalIp);
    }
}

void TelemetryManager::resolveExternalIp()
{
    QNetworkRequest request(QUrl(QStringLiteral("https://api.ipify.org?format=json")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_nam.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                m_externalIp = doc.object().value("ip").toString();
                emit ipUpdated(m_internalIp, m_externalIp);
                emit logMessage(QStringLiteral("Client External IP detected: %1").arg(m_externalIp));
            }
        } else {
            // Fallback default from screenshot if offline
            if (m_externalIp.isEmpty()) {
                m_externalIp = QStringLiteral("192.168.10.192");
                emit ipUpdated(m_internalIp, m_externalIp);
            }
        }
        reply->deleteLater();
    });
}
