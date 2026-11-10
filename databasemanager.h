#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDateTime>
#include <QRect>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;

    // Initializes and seeds schema + default admin user (creates coffeebreak.db if missing)
    bool initLocalDatabase(const QString &passwordOrKey = QString());

    // Opens an existing SQLite database WITHOUT creating it if missing (required for strict login enforcement)
    bool openExistingDatabase(const QString &dbPath = QString(), const QString &passwordOrKey = QString());

    // Resolves the path to an existing coffeebreak.db, or empty if not found
    static QString findExistingDatabaseFile();

    bool connectRemote(const QString &driver, const QString &host, int port,
                       const QString &dbName, const QString &user, const QString &password);
    void disconnectDatabase();
    bool isConnected() const;
    QString databaseFilePath() const { return m_dbFilePath; }

    // User authentication & encrypted SQLite management
    // Strictly requires an active, connected database containing the users table
    bool authenticateUser(const QString &username, const QString &password);
    bool changePassword(const QString &username, const QString &newPassword);
    bool userExists(const QString &username) const;
    bool createUser(const QString &username, const QString &password, const QString &role = QStringLiteral("admin"));

    QString currentUser() const;
    QString currentRole() const;
    static void setCurrentUser(const QString &user, const QString &role = QStringLiteral("admin"));

    // Cryptographic and encryption helpers (AES-256 / SHA-256)
    static QString hashPassword(const QString &password, const QString &salt);
    static QString generateSalt();
    static QByteArray encryptData(const QByteArray &plainText, const QString &passphrase);
    static QByteArray decryptData(const QByteArray &cipherText, const QString &passphrase);

    // Session lifecycle tracking (app_sessions table)
    int startSession(const QString &feedMode, const QString &sourcePath,
                     const QString &modelPath, const QRect &roi, const QString &digitMode);
    void updateSessionSource(const QString &feedMode, const QString &sourcePath,
                             const QRect &roi, const QString &digitMode);
    void endSession();
    int currentSessionId() const { return m_currentSessionId; }

    // Detailed Round / Lap Telemetry (roundsTable)
    struct RoundTelemetryRecord {
        int roundNumber = 0;
        QString roundLabel;
        double durationSec = 0.0;
        double deltaPrevSec = 0.0;
        double cumulativeRunSec = 0.0;
        double aiConfidence = 0.0;
        QString digitMode = QStringLiteral("Auto");
        int roiX = 1684;
        int roiY = 29;
        int roiW = 189;
        int roiH = 130;
        QString feedMode = QStringLiteral("LiveCamera");
        QString sourceName;
        qint64 videoPosMs = 0;
        double playbackSpeed = 1.0;
        double gpuTempC = 0.0;
        double gpuUtilPct = 0.0;
        int vramUsedMb = 0;
        QDateTime timestamp = QDateTime::currentDateTime();
    };

    bool logRoundDetailed(const RoundTelemetryRecord &rec);
    bool logRound(int roundNumber, double durationSec, const QDateTime &timestamp = QDateTime::currentDateTime());
    void saveRoundToCsv(int roundNumber, double durationSec, const QString &csvPath);

    // Hardware & System Event Telemetry (gpu_telemetry_log & system_event_log)
    bool logGpuSnapshot(const QString &gpuName, double tempC, double fanPct, double utilPct,
                        int memUsedMb, int memTotalMb, int currentRound, int cameraFps,
                        const QString &internalIp, const QString &externalIp);
    bool logSystemEvent(const QString &category, const QString &message);
    bool logAuthAudit(const QString &username, const QString &eventType,
                      const QString &role, const QString &details);

    QString databaseSummary() const;

    bool setIteration(int iterations);
    int iteration() const { return m_iteration; }

    int generateSampleData(int count, qint64 *elapsedMs = nullptr);
    QSqlQueryModel* loadTableData();
    bool dropTable();

    struct DbRoundRecord {
        int roundNumber = 0;
        double durationSec = 0.0;
        QDateTime timestamp;
    };
    QList<DbRoundRecord> loadRoundRecords();

signals:
    void logMessage(const QString &msg);
    void progressUpdated(int percent);

private:
    bool ensureSchemaAndSeedAdmin();

    QSqlDatabase m_db;
    QString m_dbFilePath = QStringLiteral("coffeebreak.db");
    int m_iteration = 8;
    int m_currentSessionId = 0;
    QString m_connectionName = QStringLiteral("cb_connection");
    inline static QString s_currentUser = QStringLiteral("admin");
    inline static QString s_currentRole = QStringLiteral("admin");
};

#endif // DATABASEMANAGER_H
