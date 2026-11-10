#include "databasemanager.h"

#include <QCoreApplication>
#include <QSqlError>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QElapsedTimer>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QHostInfo>
#include <QUuid>
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
    endSession();
}

QString DatabaseManager::findExistingDatabaseFile()
{
    const QStringList candidates = {
        QDir::currentPath() + QStringLiteral("/coffeebreak.db"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/coffeebreak.db"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/../coffeebreak.db"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/../../coffeebreak.db"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/../../../coffeebreak.db"),
        QStringLiteral("d:/cb26/cb26/coffeebreak.db")
    };
    for (const QString &p : candidates) {
        QFileInfo fi(p);
        if (fi.exists() && fi.isFile() && fi.size() > 0) {
            return fi.absoluteFilePath();
        }
    }
    return QString();
}

bool DatabaseManager::openExistingDatabase(const QString &dbPath, const QString &passwordOrKey)
{
    QString targetPath = dbPath.isEmpty() ? findExistingDatabaseFile() : dbPath;
    if (targetPath.isEmpty() || !QFileInfo::exists(targetPath) || QFileInfo(targetPath).size() == 0) {
        emit logMessage(QStringLiteral("DB Error: SQLite database file (coffeebreak.db) not found."));
        disconnectDatabase();
        return false;
    }

    m_dbFilePath = QFileInfo(targetPath).absoluteFilePath();

    if (QSqlDatabase::contains(m_connectionName)) {
        m_db = QSqlDatabase::database(m_connectionName, false);
        if (m_db.isOpen() && QFileInfo(m_db.databaseName()).absoluteFilePath() != m_dbFilePath) {
            m_db.close();
        }
    } else {
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    }

    m_db.setDatabaseName(m_dbFilePath);

    if (!m_db.isOpen()) {
        if (!m_db.open()) {
            emit logMessage(QStringLiteral("Error opening local database (%1): %2")
                                .arg(m_dbFilePath, m_db.lastError().text()));
            return false;
        }
    }

    if (!passwordOrKey.isEmpty()) {
        QSqlQuery keyQuery(m_db);
        keyQuery.exec(QStringLiteral("PRAGMA key = '%1';").arg(passwordOrKey));
        keyQuery.exec(QStringLiteral("PRAGMA cipher_compatibility = 4;"));
    }

    // Verify that the file is a valid SQLite database and ensure full telemetry schema exists
    QSqlQuery checkQuery(m_db);
    if (!checkQuery.exec(QStringLiteral("SELECT name FROM sqlite_master WHERE type='table' AND name='users'")) ||
        !checkQuery.next()) {
        emit logMessage(QStringLiteral("DB Error: Required 'users' table missing in %1.").arg(m_dbFilePath));
        m_db.close();
        return false;
    }

    ensureSchemaAndSeedAdmin();
    emit logMessage(QStringLiteral("DB Check: PASS, Connected to SQLite database (%1).").arg(m_dbFilePath));
    return true;
}

bool DatabaseManager::initLocalDatabase(const QString &passwordOrKey)
{
    QString existing = findExistingDatabaseFile();
    // If running inside an isolated directory (e.g. unit tests) where ./coffeebreak.db is expected,
    // use ./coffeebreak.db unless it already exists in current dir
    if (QFileInfo::exists(QStringLiteral("coffeebreak.db"))) {
        m_dbFilePath = QFileInfo(QStringLiteral("coffeebreak.db")).absoluteFilePath();
    } else if (!existing.isEmpty() && QDir::currentPath().compare(QStringLiteral("d:/cb26/cb26"), Qt::CaseInsensitive) == 0) {
        m_dbFilePath = existing;
    } else {
        m_dbFilePath = QFileInfo(QStringLiteral("coffeebreak.db")).absoluteFilePath();
    }

    if (QSqlDatabase::contains(m_connectionName)) {
        m_db = QSqlDatabase::database(m_connectionName, false);
        if (m_db.isOpen() && QFileInfo(m_db.databaseName()).absoluteFilePath() != m_dbFilePath) {
            m_db.close();
        }
    } else {
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    }

    m_db.setDatabaseName(m_dbFilePath);

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

    if (!ensureSchemaAndSeedAdmin()) {
        return false;
    }

    emit logMessage(QStringLiteral("DB Check: PASS, Local database ready (%1).").arg(m_dbFilePath));
    return true;
}

bool DatabaseManager::ensureSchemaAndSeedAdmin()
{
    if (!m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.exec(QStringLiteral("PRAGMA foreign_keys = ON;"));
    q.exec(QStringLiteral("PRAGMA journal_mode = WAL;"));

    // 1. Users table (strict database-backed authentication, including main admin account)
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password_hash TEXT NOT NULL, "
        "salt TEXT NOT NULL, "
        "role TEXT DEFAULT 'admin', "
        "is_active INTEGER DEFAULT 1, "
        "login_count INTEGER DEFAULT 0, "
        "last_login_at TEXT, "
        "created_at TEXT)"
    ));
    // Safe column migrations for older coffeebreak.db files
    q.exec(QStringLiteral("ALTER TABLE users ADD COLUMN is_active INTEGER DEFAULT 1"));
    q.exec(QStringLiteral("ALTER TABLE users ADD COLUMN login_count INTEGER DEFAULT 0"));
    q.exec(QStringLiteral("ALTER TABLE users ADD COLUMN last_login_at TEXT"));

    // 2. Authentication & Security Audit Log table
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS auth_audit_log ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "event_type TEXT NOT NULL, "
        "role TEXT, "
        "host_name TEXT, "
        "details TEXT, "
        "timestamp TEXT NOT NULL)"
    ));

    // 3. Application / Gameplay Analysis Sessions table
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS app_sessions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "session_uuid TEXT UNIQUE NOT NULL, "
        "username TEXT NOT NULL, "
        "feed_mode TEXT, "
        "source_path TEXT, "
        "model_path TEXT, "
        "roi_rect TEXT, "
        "digit_mode TEXT, "
        "start_round INTEGER DEFAULT 0, "
        "highest_round INTEGER DEFAULT 0, "
        "rounds_completed INTEGER DEFAULT 0, "
        "avg_round_duration_sec REAL DEFAULT 0.0, "
        "fastest_round_sec REAL DEFAULT 0.0, "
        "slowest_round_sec REAL DEFAULT 0.0, "
        "started_at TEXT NOT NULL, "
        "ended_at TEXT)"
    ));

    // 4. Comprehensive Rounds / Lap Telemetry table
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS roundsTable ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "session_id INTEGER, "
        "username TEXT DEFAULT 'admin', "
        "round_number INTEGER NOT NULL, "
        "round_label TEXT, "
        "duration_sec REAL NOT NULL, "
        "duration_ms INTEGER DEFAULT 0, "
        "delta_prev_sec REAL DEFAULT 0.0, "
        "cumulative_run_sec REAL DEFAULT 0.0, "
        "ai_confidence REAL DEFAULT 0.0, "
        "digit_mode TEXT DEFAULT 'Auto', "
        "roi_x INTEGER DEFAULT 1684, "
        "roi_y INTEGER DEFAULT 29, "
        "roi_w INTEGER DEFAULT 189, "
        "roi_h INTEGER DEFAULT 130, "
        "feed_mode TEXT DEFAULT 'LiveCamera', "
        "source_name TEXT, "
        "video_pos_ms INTEGER DEFAULT 0, "
        "playback_speed REAL DEFAULT 1.0, "
        "gpu_temp_c REAL DEFAULT 0.0, "
        "gpu_util_pct REAL DEFAULT 0.0, "
        "vram_used_mb INTEGER DEFAULT 0, "
        "timestamp TEXT NOT NULL)"
    ));
    // Safe column migrations in case an older 4-column roundsTable existed
    const QStringList roundAlterCols = {
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN session_id INTEGER"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN username TEXT DEFAULT 'admin'"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN round_label TEXT"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN duration_ms INTEGER DEFAULT 0"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN delta_prev_sec REAL DEFAULT 0.0"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN cumulative_run_sec REAL DEFAULT 0.0"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN ai_confidence REAL DEFAULT 0.0"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN digit_mode TEXT DEFAULT 'Auto'"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN roi_x INTEGER DEFAULT 1684"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN roi_y INTEGER DEFAULT 29"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN roi_w INTEGER DEFAULT 189"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN roi_h INTEGER DEFAULT 130"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN feed_mode TEXT DEFAULT 'LiveCamera'"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN source_name TEXT"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN video_pos_ms INTEGER DEFAULT 0"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN playback_speed REAL DEFAULT 1.0"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN gpu_temp_c REAL DEFAULT 0.0"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN gpu_util_pct REAL DEFAULT 0.0"),
        QStringLiteral("ALTER TABLE roundsTable ADD COLUMN vram_used_mb INTEGER DEFAULT 0")
    };
    for (const QString &stmt : roundAlterCols) {
        q.exec(stmt);
    }

    // 5. GPU & Network Hardware Telemetry Snapshots table
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS gpu_telemetry_log ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "session_id INTEGER, "
        "gpu_name TEXT, "
        "temp_c REAL, "
        "fan_speed_pct REAL, "
        "gpu_util_pct REAL, "
        "mem_used_mb INTEGER, "
        "mem_total_mb INTEGER, "
        "current_round INTEGER DEFAULT 0, "
        "camera_fps INTEGER DEFAULT 0, "
        "internal_ip TEXT, "
        "external_ip TEXT, "
        "timestamp TEXT NOT NULL)"
    ));

    // 6. System & AI Event Log table
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS system_event_log ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "session_id INTEGER, "
        "username TEXT, "
        "category TEXT, "
        "message TEXT NOT NULL, "
        "timestamp TEXT NOT NULL)"
    ));

    // 7. Benchmark / Wavepacket Matrix table
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS dataTable ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "x0 REAL, y0 REAL, z0 REAL, "
        "x1 REAL, y1 REAL, z1 REAL, "
        "x2 REAL, y2 REAL, z2 REAL, "
        "t INTEGER, date TEXT)"
    ));

    // Create indexes for fast query & chart loading
    q.exec(QStringLiteral("CREATE INDEX IF NOT EXISTS idx_rounds_session ON roundsTable(session_id, round_number)"));
    q.exec(QStringLiteral("CREATE INDEX IF NOT EXISTS idx_gpu_session ON gpu_telemetry_log(session_id)"));
    q.exec(QStringLiteral("CREATE INDEX IF NOT EXISTS idx_events_session ON system_event_log(session_id)"));

    // Seed default admin / admin user in SQLite if not already present
    QSqlQuery checkUser(m_db);
    checkUser.prepare(QStringLiteral("SELECT COUNT(*) FROM users WHERE username = 'admin'"));
    if (checkUser.exec() && checkUser.next()) {
        if (checkUser.value(0).toInt() == 0) {
            QString salt = generateSalt();
            QString hash = hashPassword(QStringLiteral("admin"), salt);
            QSqlQuery insertUser(m_db);
            insertUser.prepare(QStringLiteral(
                "INSERT INTO users (username, password_hash, salt, role, is_active, login_count, created_at) "
                "VALUES ('admin', :hash, :salt, 'admin', 1, 0, datetime('now'))"
            ));
            insertUser.bindValue(QStringLiteral(":hash"), hash);
            insertUser.bindValue(QStringLiteral(":salt"), salt);
            if (insertUser.exec()) {
                emit logMessage(QStringLiteral("Default administrator account initialized in SQLite (admin / admin)."));
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
    // STRICT REQUIREMENT: Never auto-create or bypass the database during login.
    // If the SQLite database file is missing or not connected, login MUST fail even for admin/admin.
    if (!m_db.isOpen() || m_dbFilePath.isEmpty() || !QFileInfo::exists(m_dbFilePath)) {
        emit logMessage(QStringLiteral("Authentication aborted: Database not found or not connected."));
        return false;
    }

    const QString cleanUser = username.trimmed();
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("SELECT password_hash, salt, role, COALESCE(is_active, 1) FROM users WHERE username = :u"));
    q.bindValue(QStringLiteral(":u"), cleanUser);

    if (!q.exec() || !q.next()) {
        logAuthAudit(cleanUser, QStringLiteral("LOGIN_FAILED"), QString(), QStringLiteral("User not found in SQLite users table"));
        emit logMessage(QStringLiteral("Authentication rejected: User '%1' not found.").arg(cleanUser));
        return false;
    }

    const QString storedHash = q.value(0).toString();
    const QString salt = q.value(1).toString();
    const QString role = q.value(2).toString();
    const int isActive = q.value(3).toInt();

    if (isActive == 0) {
        logAuthAudit(cleanUser, QStringLiteral("LOGIN_FAILED"), role, QStringLiteral("Account is disabled (is_active = 0)"));
        emit logMessage(QStringLiteral("Authentication rejected: Account '%1' is disabled.").arg(cleanUser));
        return false;
    }

    const QString testHash = hashPassword(password, salt);
    if (testHash == storedHash) {
        s_currentUser = cleanUser;
        s_currentRole = role;

        // Update login count and timestamp in users table
        QSqlQuery upd(m_db);
        upd.prepare(QStringLiteral(
            "UPDATE users SET login_count = COALESCE(login_count, 0) + 1, "
            "last_login_at = :ts WHERE username = :u"
        ));
        upd.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(Qt::ISODate));
        upd.bindValue(QStringLiteral(":u"), cleanUser);
        upd.exec();

        logAuthAudit(cleanUser, QStringLiteral("LOGIN_SUCCESS"), role, QStringLiteral("Authenticated via SQLite SHA-256 credentials"));
        emit logMessage(QStringLiteral("User '%1' authenticated successfully with role '%2'.").arg(s_currentUser, s_currentRole));
        return true;
    }

    logAuthAudit(cleanUser, QStringLiteral("LOGIN_FAILED"), role, QStringLiteral("Invalid password hash mismatch"));
    emit logMessage(QStringLiteral("Authentication rejected: Invalid password for user '%1'.").arg(cleanUser));
    return false;
}

bool DatabaseManager::logAuthAudit(const QString &username, const QString &eventType,
                                   const QString &role, const QString &details)
{
    if (!m_db.isOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO auth_audit_log (username, event_type, role, host_name, details, timestamp) "
        "VALUES (:u, :ev, :r, :h, :d, :ts)"
    ));
    q.bindValue(QStringLiteral(":u"), username.isEmpty() ? QStringLiteral("unknown") : username);
    q.bindValue(QStringLiteral(":ev"), eventType);
    q.bindValue(QStringLiteral(":r"), role);
    q.bindValue(QStringLiteral(":h"), QHostInfo::localHostName());
    q.bindValue(QStringLiteral(":d"), details);
    q.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(Qt::ISODate));
    return q.exec();
}

bool DatabaseManager::changePassword(const QString &username, const QString &newPassword)
{
    if (!m_db.isOpen()) return false;

    QString salt = generateSalt();
    QString hash = hashPassword(newPassword, salt);

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("UPDATE users SET password_hash = :hash, salt = :salt WHERE username = :u"));
    q.bindValue(QStringLiteral(":hash"), hash);
    q.bindValue(QStringLiteral(":salt"), salt);
    q.bindValue(QStringLiteral(":u"), username.trimmed());
    bool ok = q.exec() && q.numRowsAffected() > 0;
    if (ok) {
        logAuthAudit(username.trimmed(), QStringLiteral("PASSWORD_CHANGED"), s_currentRole, QStringLiteral("User password updated"));
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
    if (!m_db.isOpen()) return false;

    QString salt = generateSalt();
    QString hash = hashPassword(password, salt);

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO users (username, password_hash, salt, role, is_active, login_count, created_at) "
        "VALUES (:u, :hash, :salt, :role, 1, 0, datetime('now'))"
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

int DatabaseManager::startSession(const QString &feedMode, const QString &sourcePath,
                                  const QString &modelPath, const QRect &roi, const QString &digitMode)
{
    if (!m_db.isOpen()) return 0;

    if (m_currentSessionId > 0) {
        endSession();
    }

    const QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString roiStr = QString::asprintf("%d,%d,%d,%d", roi.x(), roi.y(), roi.width(), roi.height());

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO app_sessions (session_uuid, username, feed_mode, source_path, model_path, roi_rect, digit_mode, started_at) "
        "VALUES (:uuid, :u, :fm, :src, :mp, :roi, :dm, :ts)"
    ));
    q.bindValue(QStringLiteral(":uuid"), uuid);
    q.bindValue(QStringLiteral(":u"), s_currentUser);
    q.bindValue(QStringLiteral(":fm"), feedMode);
    q.bindValue(QStringLiteral(":src"), sourcePath);
    q.bindValue(QStringLiteral(":mp"), modelPath);
    q.bindValue(QStringLiteral(":roi"), roiStr);
    q.bindValue(QStringLiteral(":dm"), digitMode);
    q.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(Qt::ISODate));

    if (q.exec()) {
        m_currentSessionId = q.lastInsertId().toInt();
        return m_currentSessionId;
    }
    return 0;
}

void DatabaseManager::updateSessionSource(const QString &feedMode, const QString &sourcePath,
                                          const QRect &roi, const QString &digitMode)
{
    if (!m_db.isOpen()) return;
    if (m_currentSessionId <= 0) {
        startSession(feedMode, sourcePath, QString(), roi, digitMode);
        return;
    }
    const QString roiStr = QString::asprintf("%d,%d,%d,%d", roi.x(), roi.y(), roi.width(), roi.height());
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "UPDATE app_sessions SET feed_mode = :fm, source_path = :src, roi_rect = :roi, digit_mode = :dm "
        "WHERE id = :id"
    ));
    q.bindValue(QStringLiteral(":fm"), feedMode);
    q.bindValue(QStringLiteral(":src"), sourcePath);
    q.bindValue(QStringLiteral(":roi"), roiStr);
    q.bindValue(QStringLiteral(":dm"), digitMode);
    q.bindValue(QStringLiteral(":id"), m_currentSessionId);
    q.exec();
}

void DatabaseManager::endSession()
{
    if (!m_db.isOpen() || m_currentSessionId <= 0) return;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("UPDATE app_sessions SET ended_at = :ts WHERE id = :id AND ended_at IS NULL"));
    q.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(Qt::ISODate));
    q.bindValue(QStringLiteral(":id"), m_currentSessionId);
    q.exec();
    m_currentSessionId = 0;
}

bool DatabaseManager::logRoundDetailed(const RoundTelemetryRecord &rec)
{
    if (!m_db.isOpen()) return false;

    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO roundsTable ("
        "session_id, username, round_number, round_label, duration_sec, duration_ms, "
        "delta_prev_sec, cumulative_run_sec, ai_confidence, digit_mode, "
        "roi_x, roi_y, roi_w, roi_h, feed_mode, source_name, video_pos_ms, playback_speed, "
        "gpu_temp_c, gpu_util_pct, vram_used_mb, timestamp"
        ") VALUES ("
        ":sid, :u, :rn, :rl, :dsec, :dms, "
        ":delta, :cum, :conf, :dm, "
        ":rx, :ry, :rw, :rh, :fm, :src, :vpos, :spd, "
        ":gtemp, :gutil, :vram, :ts"
        ")"
    ));
    q.bindValue(QStringLiteral(":sid"), m_currentSessionId > 0 ? QVariant(m_currentSessionId) : QVariant());
    q.bindValue(QStringLiteral(":u"), s_currentUser);
    q.bindValue(QStringLiteral(":rn"), rec.roundNumber);
    q.bindValue(QStringLiteral(":rl"), rec.roundLabel.isEmpty() ? QString::number(rec.roundNumber) : rec.roundLabel);
    q.bindValue(QStringLiteral(":dsec"), rec.durationSec);
    q.bindValue(QStringLiteral(":dms"), static_cast<qint64>(rec.durationSec * 1000.0));
    q.bindValue(QStringLiteral(":delta"), rec.deltaPrevSec);
    q.bindValue(QStringLiteral(":cum"), rec.cumulativeRunSec);
    q.bindValue(QStringLiteral(":conf"), rec.aiConfidence);
    q.bindValue(QStringLiteral(":dm"), rec.digitMode);
    q.bindValue(QStringLiteral(":rx"), rec.roiX);
    q.bindValue(QStringLiteral(":ry"), rec.roiY);
    q.bindValue(QStringLiteral(":rw"), rec.roiW);
    q.bindValue(QStringLiteral(":rh"), rec.roiH);
    q.bindValue(QStringLiteral(":fm"), rec.feedMode);
    q.bindValue(QStringLiteral(":src"), rec.sourceName);
    q.bindValue(QStringLiteral(":vpos"), rec.videoPosMs);
    q.bindValue(QStringLiteral(":spd"), rec.playbackSpeed);
    q.bindValue(QStringLiteral(":gtemp"), rec.gpuTempC);
    q.bindValue(QStringLiteral(":gutil"), rec.gpuUtilPct);
    q.bindValue(QStringLiteral(":vram"), rec.vramUsedMb);
    q.bindValue(QStringLiteral(":ts"), rec.timestamp.toString(Qt::ISODate));

    bool ok = q.exec();

    // Update session aggregates in app_sessions
    if (ok && m_currentSessionId > 0) {
        QSqlQuery upd(m_db);
        upd.prepare(QStringLiteral(
            "UPDATE app_sessions SET "
            "start_round = CASE WHEN start_round = 0 THEN :rn ELSE start_round END, "
            "highest_round = MAX(highest_round, :rn), "
            "rounds_completed = (SELECT COUNT(*) FROM roundsTable WHERE session_id = :sid), "
            "avg_round_duration_sec = (SELECT COALESCE(AVG(duration_sec), 0.0) FROM roundsTable WHERE session_id = :sid), "
            "fastest_round_sec = (SELECT COALESCE(MIN(duration_sec), 0.0) FROM roundsTable WHERE session_id = :sid), "
            "slowest_round_sec = (SELECT COALESCE(MAX(duration_sec), 0.0) FROM roundsTable WHERE session_id = :sid) "
            "WHERE id = :sid"
        ));
        upd.bindValue(QStringLiteral(":rn"), rec.roundNumber);
        upd.bindValue(QStringLiteral(":sid"), m_currentSessionId);
        upd.exec();
    }

    return ok;
}

bool DatabaseManager::logRound(int roundNumber, double durationSec, const QDateTime &timestamp)
{
    RoundTelemetryRecord rec;
    rec.roundNumber = roundNumber;
    rec.roundLabel = QString::number(roundNumber);
    rec.durationSec = durationSec;
    rec.timestamp = timestamp;
    return logRoundDetailed(rec);
}

bool DatabaseManager::logGpuSnapshot(const QString &gpuName, double tempC, double fanPct, double utilPct,
                                     int memUsedMb, int memTotalMb, int currentRound, int cameraFps,
                                     const QString &internalIp, const QString &externalIp)
{
    if (!m_db.isOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO gpu_telemetry_log ("
        "session_id, gpu_name, temp_c, fan_speed_pct, gpu_util_pct, "
        "mem_used_mb, mem_total_mb, current_round, camera_fps, internal_ip, external_ip, timestamp"
        ") VALUES (:sid, :gn, :tc, :fan, :ut, :mu, :mt, :cr, :fps, :iip, :eip, :ts)"
    ));
    q.bindValue(QStringLiteral(":sid"), m_currentSessionId > 0 ? QVariant(m_currentSessionId) : QVariant());
    q.bindValue(QStringLiteral(":gn"), gpuName);
    q.bindValue(QStringLiteral(":tc"), tempC);
    q.bindValue(QStringLiteral(":fan"), fanPct);
    q.bindValue(QStringLiteral(":ut"), utilPct);
    q.bindValue(QStringLiteral(":mu"), memUsedMb);
    q.bindValue(QStringLiteral(":mt"), memTotalMb);
    q.bindValue(QStringLiteral(":cr"), currentRound);
    q.bindValue(QStringLiteral(":fps"), cameraFps);
    q.bindValue(QStringLiteral(":iip"), internalIp);
    q.bindValue(QStringLiteral(":eip"), externalIp);
    q.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(Qt::ISODate));
    return q.exec();
}

bool DatabaseManager::logSystemEvent(const QString &category, const QString &message)
{
    if (!m_db.isOpen()) return false;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO system_event_log (session_id, username, category, message, timestamp) "
        "VALUES (:sid, :u, :cat, :msg, :ts)"
    ));
    q.bindValue(QStringLiteral(":sid"), m_currentSessionId > 0 ? QVariant(m_currentSessionId) : QVariant());
    q.bindValue(QStringLiteral(":u"), s_currentUser);
    q.bindValue(QStringLiteral(":cat"), category);
    q.bindValue(QStringLiteral(":msg"), message);
    q.bindValue(QStringLiteral(":ts"), QDateTime::currentDateTime().toString(Qt::ISODate));
    return q.exec();
}

QString DatabaseManager::databaseSummary() const
{
    if (!m_db.isOpen()) {
        return QStringLiteral("Database: Disconnected");
    }

    auto countTable = [this](const QString &table) -> int {
        QSqlQuery q(m_db);
        if (q.exec(QStringLiteral("SELECT COUNT(*) FROM %1").arg(table)) && q.next()) {
            return q.value(0).toInt();
        }
        return 0;
    };

    int usersCount = countTable(QStringLiteral("users"));
    int sessionsCount = countTable(QStringLiteral("app_sessions"));
    int roundsCount = countTable(QStringLiteral("roundsTable"));
    int gpuCount = countTable(QStringLiteral("gpu_telemetry_log"));
    int eventsCount = countTable(QStringLiteral("system_event_log"));
    int auditCount = countTable(QStringLiteral("auth_audit_log"));

    QString lastLogin = QStringLiteral("N/A");
    int loginCount = 0;
    QSqlQuery uq(m_db);
    uq.prepare(QStringLiteral("SELECT COALESCE(login_count, 0), COALESCE(last_login_at, 'N/A') FROM users WHERE username = :u"));
    uq.bindValue(QStringLiteral(":u"), s_currentUser);
    if (uq.exec() && uq.next()) {
        loginCount = uq.value(0).toInt();
        lastLogin = uq.value(1).toString();
    }

    return QStringLiteral(
        "SQLite Database: %1\n"
        "User: %2 (Role: %3 | Logins: %4 | Last Login: %5)\n"
        "Active Session ID: %6\n"
        "Recorded Tables:\n"
        "  • users: %7 account(s)\n"
        "  • auth_audit_log: %8 security event(s)\n"
        "  • app_sessions: %9 session(s)\n"
        "  • roundsTable: %10 round telemetry record(s)\n"
        "  • gpu_telemetry_log: %11 hardware snapshot(s)\n"
        "  • system_event_log: %12 system/AI event(s)")
        .arg(m_dbFilePath)
        .arg(s_currentUser, s_currentRole)
        .arg(loginCount)
        .arg(lastLogin)
        .arg(m_currentSessionId)
        .arg(usersCount)
        .arg(auditCount)
        .arg(sessionsCount)
        .arg(roundsCount)
        .arg(gpuCount)
        .arg(eventsCount);
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
        plainText.resize(outSize);
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
        endSession();
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
        return 0;
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
    QSqlQueryModel *model = new QSqlQueryModel();
    if (!m_db.isOpen()) {
        emit logMessage(QStringLiteral("Query error: Database is not connected."));
        return model;
    }

    model->setQuery(QStringLiteral("SELECT * FROM roundsTable ORDER BY id DESC LIMIT 500"), m_db);

    if (model->lastError().isValid()) {
        emit logMessage(QStringLiteral("Query error: %1").arg(model->lastError().text()));
    } else {
        emit logMessage(QStringLiteral("Loaded %1 records from roundsTable.").arg(model->rowCount()));
    }

    return model;
}

bool DatabaseManager::dropTable()
{
    if (!m_db.isOpen()) {
        return false;
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

QList<DatabaseManager::DbRoundRecord> DatabaseManager::loadRoundRecords()
{
    QList<DbRoundRecord> list;
    if (!m_db.isOpen()) {
        return list;
    }
    QSqlQuery q(m_db);
    if (q.exec(QStringLiteral("SELECT round_number, duration_sec, timestamp FROM roundsTable ORDER BY id ASC"))) {
        while (q.next()) {
            DbRoundRecord rec;
            rec.roundNumber = q.value(0).toInt();
            rec.durationSec = q.value(1).toDouble();
            rec.timestamp = QDateTime::fromString(q.value(2).toString(), Qt::ISODate);
            list.append(rec);
        }
    }
    return list;
}
