#include "databasemanager.h"

#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <random>

#ifdef Q_OS_WIN
#include <windows.h>
#include <bcrypt.h>
#endif

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    disconnectDatabase();
}

bool DatabaseManager::initLocalDatabase(const QString &passwordOrKey)
{
    if (QSqlDatabase::contains(m_connectionName)) {
        m_db = QSqlDatabase::database(m_connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
        m_db.setDatabaseName(QStringLiteral("coffeebreak.db"));
    }

    if (!m_db.isOpen()) {
        if (!m_db.open()) {
            emit logMessage(QStringLiteral("Error opening local database: %1").arg(m_db.lastError().text()));
            return false;
        }
    }

    // Apply encryption key if specified (SQLCipher / SEE compatible)
    if (!passwordOrKey.isEmpty()) {
        QSqlQuery keyQuery(m_db);
        keyQuery.exec(QStringLiteral("PRAGMA key = '%1';").arg(passwordOrKey));
        keyQuery.exec(QStringLiteral("PRAGMA cipher_compatibility = 4;"));
    }

    emit logMessage(QStringLiteral("DB Check: PASS, Local database found (coffeebreak.db)."));

    // Ensure tables exist
    QSqlQuery q(m_db);
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS dataTable ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "x0 REAL, y0 REAL, z0 REAL, "
        "x1 REAL, y1 REAL, z1 REAL, "
        "x2 REAL, y2 REAL, z2 REAL, "
        "t INTEGER, date TEXT)"
    ));

    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS roundsTable ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "round_number INTEGER, "
        "duration_sec REAL, "
        "timestamp TEXT)"
    ));

    // Create users table for encrypted/salted credential authentication
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password_hash TEXT NOT NULL, "
        "salt TEXT NOT NULL, "
        "role TEXT DEFAULT 'admin', "
        "created_at TEXT)"
    ));

    // Ensure default admin / admin user exists
    QSqlQuery checkUser(m_db);
    checkUser.prepare(QStringLiteral("SELECT COUNT(*) FROM users WHERE username = 'admin'"));
    if (checkUser.exec() && checkUser.next()) {
        if (checkUser.value(0).toInt() == 0) {
            QString salt = generateSalt();
            QString hash = hashPassword(QStringLiteral("admin"), salt);
            QSqlQuery insertUser(m_db);
            insertUser.prepare(QStringLiteral(
                "INSERT INTO users (username, password_hash, salt, role, created_at) "
                "VALUES ('admin', :hash, :salt, 'admin', datetime('now'))"
            ));
            insertUser.bindValue(QStringLiteral(":hash"), hash);
            insertUser.bindValue(QStringLiteral(":salt"), salt);
            if (insertUser.exec()) {
                emit logMessage(QStringLiteral("Default administrator account initialized (admin / admin)."));
            }
        }
    }

    return true;
}

QString DatabaseManager::hashPassword(const QString &password, const QString &salt)
{
    QByteArray combined = salt.toUtf8() + password.toUtf8();
    return QString::fromLatin1(QCryptographicHash::hash(combined, QCryptographicHash::Sha256).toHex());
}

QString DatabaseManager::generateSalt()
{
    QByteArray bytes;
    bytes.resize(16);
    for (int i = 0; i < 16; ++i) {
        bytes[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    return QString::fromLatin1(bytes.toHex());
}

bool DatabaseManager::authenticateUser(const QString &username, const QString &password)
{
    if (!m_db.isOpen()) {
        if (!initLocalDatabase()) {
            return false;
        }
    }

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT password_hash, salt, role FROM users WHERE username = :u"));
    q.bindValue(QStringLiteral(":u"), username.trimmed());

    if (!q.exec() || !q.next()) {
        emit logMessage(QStringLiteral("Authentication rejected: User '%1' not found.").arg(username));
        return false;
    }

    QString storedHash = q.value(0).toString();
    QString salt = q.value(1).toString();
    QString role = q.value(2).toString();

    QString testHash = hashPassword(password, salt);
    if (testHash == storedHash) {
        s_currentUser = username.trimmed();
        s_currentRole = role;
        emit logMessage(QStringLiteral("User '%1' authenticated successfully with role '%2'.").arg(s_currentUser, s_currentRole));
        return true;
    }

    emit logMessage(QStringLiteral("Authentication rejected: Invalid password for user '%1'.").arg(username));
    return false;
}

bool DatabaseManager::changePassword(const QString &username, const QString &newPassword)
{
    if (!m_db.isOpen()) {
        if (!initLocalDatabase()) return false;
    }

    QString salt = generateSalt();
    QString hash = hashPassword(newPassword, salt);

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("UPDATE users SET password_hash = :hash, salt = :salt WHERE username = :u"));
    q.bindValue(QStringLiteral(":hash"), hash);
    q.bindValue(QStringLiteral(":salt"), salt);
    q.bindValue(QStringLiteral(":u"), username.trimmed());
    bool ok = q.exec();
    if (ok) {
        emit logMessage(QStringLiteral("Password changed successfully for user '%1'.").arg(username));
    }
    return ok;
}

bool DatabaseManager::userExists(const QString &username) const
{
    if (!m_db.isOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT COUNT(*) FROM users WHERE username = :u"));
    q.bindValue(QStringLiteral(":u"), username.trimmed());
    if (q.exec() && q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::createUser(const QString &username, const QString &password, const QString &role)
{
    if (!m_db.isOpen()) {
        if (!initLocalDatabase()) return false;
    }

    QString salt = generateSalt();
    QString hash = hashPassword(password, salt);

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO users (username, password_hash, salt, role, created_at) "
        "VALUES (:u, :hash, :salt, :role, datetime('now'))"
    ));
    q.bindValue(QStringLiteral(":u"), username.trimmed());
    q.bindValue(QStringLiteral(":hash"), hash);
    q.bindValue(QStringLiteral(":salt"), salt);
    q.bindValue(QStringLiteral(":role"), role);
    return q.exec();
}

QString DatabaseManager::currentUser() const
{
    return s_currentUser;
}

QString DatabaseManager::currentRole() const
{
    return s_currentRole;
}

void DatabaseManager::setCurrentUser(const QString &user, const QString &role)
{
    s_currentUser = user;
    s_currentRole = role;
}

QByteArray DatabaseManager::encryptData(const QByteArray &plainText, const QString &passphrase)
{
#ifdef Q_OS_WIN
    BCRYPT_ALG_HANDLE hAlg = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
    if (!BCRYPT_SUCCESS(status)) return QByteArray();

    status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
    if (!BCRYPT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return QByteArray();
    }

    QByteArray keyBytes = QCryptographicHash::hash(passphrase.toUtf8(), QCryptographicHash::Sha256);
    QByteArray ivBytes;
    ivBytes.resize(16);
    for (int i = 0; i < 16; ++i) {
        ivBytes[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }

    BCRYPT_KEY_HANDLE hKey = NULL;
    status = BCryptGenerateSymmetricKey(hAlg, &hKey, NULL, 0, (PUCHAR)keyBytes.data(), 32, 0);
    if (!BCRYPT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return QByteArray();
    }

    QByteArray ivCopy = ivBytes;
    DWORD outSize = 0;
    status = BCryptEncrypt(hKey, (PUCHAR)plainText.data(), (ULONG)plainText.size(), NULL, (PUCHAR)ivCopy.data(), 16, NULL, 0, &outSize, BCRYPT_BLOCK_PADDING);
    if (!BCRYPT_SUCCESS(status) || outSize == 0) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return QByteArray();
    }

    QByteArray cipherText;
    cipherText.resize(outSize);
    ivCopy = ivBytes;
    status = BCryptEncrypt(hKey, (PUCHAR)plainText.data(), (ULONG)plainText.size(), NULL, (PUCHAR)ivCopy.data(), 16, (PUCHAR)cipherText.data(), outSize, &outSize, BCRYPT_BLOCK_PADDING);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    if (BCRYPT_SUCCESS(status)) {
        return ivBytes + cipherText;
    }
    return QByteArray();
#else
    return plainText;
#endif
}

QByteArray DatabaseManager::decryptData(const QByteArray &cipherTextWithIv, const QString &passphrase)
{
#ifdef Q_OS_WIN
    if (cipherTextWithIv.size() <= 16) return QByteArray();

    QByteArray ivBytes = cipherTextWithIv.left(16);
    QByteArray cipherText = cipherTextWithIv.mid(16);

    BCRYPT_ALG_HANDLE hAlg = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
    if (!BCRYPT_SUCCESS(status)) return QByteArray();

    status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
    if (!BCRYPT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return QByteArray();
    }

    QByteArray keyBytes = QCryptographicHash::hash(passphrase.toUtf8(), QCryptographicHash::Sha256);
    BCRYPT_KEY_HANDLE hKey = NULL;
    status = BCryptGenerateSymmetricKey(hAlg, &hKey, NULL, 0, (PUCHAR)keyBytes.data(), 32, 0);
    if (!BCRYPT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return QByteArray();
    }

    QByteArray ivCopy = ivBytes;
    DWORD outSize = 0;
    status = BCryptDecrypt(hKey, (PUCHAR)cipherText.data(), (ULONG)cipherText.size(), NULL, (PUCHAR)ivCopy.data(), 16, NULL, 0, &outSize, BCRYPT_BLOCK_PADDING);
    if (!BCRYPT_SUCCESS(status) || outSize == 0) {
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return QByteArray();
    }

    QByteArray plainText;
    plainText.resize(outSize);
    ivCopy = ivBytes;
    status = BCryptDecrypt(hKey, (PUCHAR)cipherText.data(), (ULONG)cipherText.size(), NULL, (PUCHAR)ivCopy.data(), 16, (PUCHAR)plainText.data(), outSize, &outSize, BCRYPT_BLOCK_PADDING);

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    if (BCRYPT_SUCCESS(status)) {
        return plainText;
    }
    return QByteArray();
#else
    return cipherTextWithIv;
#endif
}

bool DatabaseManager::connectRemote(const QString &driver, const QString &host, int port,
                                   const QString &dbName, const QString &user, const QString &password)
{
    disconnectDatabase();

    emit logMessage(QStringLiteral("Connecting to %1 remote database server (%2:%3)...")
                    .arg(driver).arg(host).arg(port));

    m_db = QSqlDatabase::addDatabase(driver, m_connectionName);
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(password);

    if (!m_db.open()) {
        emit logMessage(QStringLiteral("Error: '%1'").arg(m_db.lastError().text()));
        return false;
    }

    emit logMessage(QStringLiteral("Connected to %1 remote database successfully.").arg(driver));
    return true;
}

void DatabaseManager::disconnectDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
        emit logMessage(QStringLiteral("Database connection closed."));
    }
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

bool DatabaseManager::setIteration(int iterations)
{
    if (iterations < 1 || iterations > 1000000) {
        emit logMessage(QStringLiteral("Invalid iteration value. Accepted range: 1 to 1,000,000."));
        return false;
    }
    m_iteration = iterations;
    emit logMessage(QStringLiteral("New value of database records set to: %1").arg(iterations));
    return true;
}

int DatabaseManager::generateSampleData(int count, qint64 *elapsedMs)
{
    if (!m_db.isOpen()) {
        initLocalDatabase();
    }

    QElapsedTimer timer;
    timer.start();

    m_db.transaction();

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO dataTable (x0, y0, z0, x1, y1, z1, x2, y2, z2, t, date) "
        "VALUES (:x0, :y0, :z0, :x1, :y1, :z1, :x2, :y2, :z2, :t, datetime('now'))"
    ));

    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(10, 99);

    for (int i = 0; i < count; ++i) {
        q.bindValue(QStringLiteral(":x0"), distr(eng));
        q.bindValue(QStringLiteral(":y0"), distr(eng));
        q.bindValue(QStringLiteral(":z0"), distr(eng));
        q.bindValue(QStringLiteral(":x1"), distr(eng));
        q.bindValue(QStringLiteral(":y1"), distr(eng));
        q.bindValue(QStringLiteral(":z1"), distr(eng));
        q.bindValue(QStringLiteral(":x2"), distr(eng));
        q.bindValue(QStringLiteral(":y2"), distr(eng));
        q.bindValue(QStringLiteral(":z2"), distr(eng));
        q.bindValue(QStringLiteral(":t"), static_cast<int>(timer.elapsed()));
        q.exec();

        if (count > 100 && (i % (count / 10) == 0)) {
            emit progressUpdated(static_cast<int>((i * 100.0) / count));
        }
    }

    m_db.commit();
    emit progressUpdated(100);

    qint64 totalMs = timer.elapsed();
    if (elapsedMs) *elapsedMs = totalMs;

    emit logMessage(QStringLiteral("DATA insertion successful. Inserted %1 records in %2 ms (%3 sec).")
                    .arg(count).arg(totalMs).arg(totalMs / 1000.0, 0, 'f', 2));

    return count;
}

QSqlQueryModel* DatabaseManager::loadTableData()
{
    if (!m_db.isOpen()) {
        initLocalDatabase();
    }

    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(QStringLiteral("SELECT * FROM dataTable ORDER BY ID DESC LIMIT 500"), m_db);

    if (model->lastError().isValid()) {
        emit logMessage(QStringLiteral("Query error: %1").arg(model->lastError().text()));
    } else {
        emit logMessage(QStringLiteral("Loaded %1 records from dataTable.").arg(model->rowCount()));
    }

    return model;
}

bool DatabaseManager::dropTable()
{
    if (!m_db.isOpen()) {
        initLocalDatabase();
    }

    QSqlQuery q(m_db);
    bool ok = q.exec(QStringLiteral("DELETE FROM dataTable"));
    if (ok) {
        emit logMessage(QStringLiteral("Database table dataTable cleared successfully."));
    } else {
        emit logMessage(QStringLiteral("Error dropping/clearing table: %1").arg(q.lastError().text()));
    }
    return ok;
}

bool DatabaseManager::logRound(int roundNumber, double durationSec, const QDateTime &timestamp)
{
    if (!m_db.isOpen()) {
        initLocalDatabase();
    }

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO roundsTable (round_number, duration_sec, timestamp) "
        "VALUES (:r, :d, :ts)"
    ));
    q.bindValue(QStringLiteral(":r"), roundNumber);
    q.bindValue(QStringLiteral(":d"), durationSec);
    q.bindValue(QStringLiteral(":ts"), timestamp.toString(Qt::ISODate));

    return q.exec();
}

void DatabaseManager::saveRoundToCsv(int roundNumber, double durationSec, const QString &csvPath)
{
    QFile file(csvPath);
    bool writeHeader = !file.exists();

    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        if (writeHeader) {
            out << "Round,Timestamp,DurationMs\n";
        }
        out << roundNumber << ","
            << QDateTime::currentDateTime().toString(Qt::ISODate) << ","
            << static_cast<qint64>(durationSec * 1000.0) << "\n";
        file.close();
    }
}
