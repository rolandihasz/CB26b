#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDateTime>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;

    bool initLocalDatabase(const QString &passwordOrKey = QString());
    bool connectRemote(const QString &driver, const QString &host, int port,
                       const QString &dbName, const QString &user, const QString &password);
    void disconnectDatabase();
    bool isConnected() const;

    // User authentication & encrypted SQLite management
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

    bool setIteration(int iterations);
    int iteration() const { return m_iteration; }

    int generateSampleData(int count, qint64 *elapsedMs = nullptr);
    QSqlQueryModel* loadTableData();
    bool dropTable();

    bool logRound(int roundNumber, double durationSec, const QDateTime &timestamp = QDateTime::currentDateTime());
    void saveRoundToCsv(int roundNumber, double durationSec, const QString &csvPath);

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
    QSqlDatabase m_db;
    int m_iteration = 8;
    QString m_connectionName = QStringLiteral("cb_connection");
    inline static QString s_currentUser = QStringLiteral("admin");
    inline static QString s_currentRole = QStringLiteral("admin");
};

#endif // DATABASEMANAGER_H
