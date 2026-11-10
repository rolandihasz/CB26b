#include <gtest/gtest.h>
#include "databasemanager.h"
#include <QTemporaryDir>
#include <QDir>
#include <QCoreApplication>
#include <QSqlDatabase>

class DatabaseManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Run tests inside an isolated temporary directory so production DB is untouched
        ASSERT_TRUE(m_tempDir.isValid());
        m_originalDir = QDir::currentPath();
        QDir::setCurrent(m_tempDir.path());
    }

    void TearDown() override {
        // Disconnect DB and release SQLite file lock before cleaning temp dir
        {
            DatabaseManager db;
            db.disconnectDatabase();
        }
        if (QSqlDatabase::contains(QStringLiteral("cb_connection"))) {
            QSqlDatabase::removeDatabase(QStringLiteral("cb_connection"));
        }
        QDir::setCurrent(m_originalDir);
    }

    QTemporaryDir m_tempDir;
    QString m_originalDir;
};

// ----------------------------------------------------------------------------
// Cryptography and Hash Tests
// ----------------------------------------------------------------------------

TEST_F(DatabaseManagerTest, SaltGenerationLengthAndRandomness) {
    QString salt1 = DatabaseManager::generateSalt();
    QString salt2 = DatabaseManager::generateSalt();

    // 16 bytes encoded as hex string -> 32 hex characters
    EXPECT_EQ(salt1.length(), 32);
    EXPECT_EQ(salt2.length(), 32);
    EXPECT_NE(salt1, salt2);
}

TEST_F(DatabaseManagerTest, PasswordHashingConsistencyAndSeparation) {
    QString saltA = QStringLiteral("0123456789abcdef0123456789abcdef");
    QString saltB = QStringLiteral("fedcba9876543210fedcba9876543210");
    QString pass1 = QStringLiteral("Formula1LapTimer");
    QString pass2 = QStringLiteral("Formula2LapTimer");

    QString hash1 = DatabaseManager::hashPassword(pass1, saltA);
    QString hash1_repeat = DatabaseManager::hashPassword(pass1, saltA);
    QString hashDifferentSalt = DatabaseManager::hashPassword(pass1, saltB);
    QString hashDifferentPass = DatabaseManager::hashPassword(pass2, saltA);

    // SHA-256 output is 64 hex characters
    EXPECT_EQ(hash1.length(), 64);
    EXPECT_EQ(hash1, hash1_repeat);
    EXPECT_NE(hash1, hashDifferentSalt);
    EXPECT_NE(hash1, hashDifferentPass);
}

TEST_F(DatabaseManagerTest, Aes256EncryptDecryptRoundtrip) {
    QByteArray plainText = "Telemetry packet telemetry_id=9843 speed=284.5 km/h lap=14";
    QString passphrase = QStringLiteral("SuperSecretPassphrase2026!");

    QByteArray encrypted = DatabaseManager::encryptData(plainText, passphrase);
    ASSERT_FALSE(encrypted.isEmpty());
    EXPECT_NE(encrypted, plainText);

    QByteArray decrypted = DatabaseManager::decryptData(encrypted, passphrase);
    EXPECT_EQ(decrypted, plainText);
}

TEST_F(DatabaseManagerTest, Aes256DecryptWithWrongKeyOrCorruptedData) {
    QByteArray plainText = "Confidential telemetry data";
    QString correctKey = QStringLiteral("CorrectKey");
    QString wrongKey = QStringLiteral("WrongKey");

    QByteArray encrypted = DatabaseManager::encryptData(plainText, correctKey);
    ASSERT_FALSE(encrypted.isEmpty());

    QByteArray decryptedWrongKey = DatabaseManager::decryptData(encrypted, wrongKey);
    EXPECT_NE(decryptedWrongKey, plainText);

    // Truncated/corrupted ciphertext
    QByteArray corrupted = encrypted.left(10);
    QByteArray decryptedCorrupted = DatabaseManager::decryptData(corrupted, correctKey);
    EXPECT_TRUE(decryptedCorrupted.isEmpty());
}

// ----------------------------------------------------------------------------
// Database Operations & Authentication Tests
// ----------------------------------------------------------------------------

TEST_F(DatabaseManagerTest, IterationLimits) {
    DatabaseManager db;
    EXPECT_EQ(db.iteration(), 8); // Default iteration count

    EXPECT_TRUE(db.setIteration(100));
    EXPECT_EQ(db.iteration(), 100);

    EXPECT_FALSE(db.setIteration(0));
    EXPECT_FALSE(db.setIteration(-1));
    EXPECT_FALSE(db.setIteration(2000000));
}

TEST_F(DatabaseManagerTest, DatabaseInitAndDefaultAdmin) {
    DatabaseManager db;
    EXPECT_FALSE(db.isConnected());

    ASSERT_TRUE(db.initLocalDatabase());
    EXPECT_TRUE(db.isConnected());

    // Default admin account must exist and be authenticatable
    EXPECT_TRUE(db.userExists(QStringLiteral("admin")));
    EXPECT_TRUE(db.authenticateUser(QStringLiteral("admin"), QStringLiteral("admin")));
    EXPECT_FALSE(db.authenticateUser(QStringLiteral("admin"), QStringLiteral("wrongpassword")));
    EXPECT_FALSE(db.authenticateUser(QStringLiteral("nonexistent"), QStringLiteral("admin")));
}

TEST_F(DatabaseManagerTest, UserCreationAndAuthenticationFlow) {
    DatabaseManager db;
    ASSERT_TRUE(db.initLocalDatabase());

    QString username = QStringLiteral("telemetry_operator");
    QString password = QStringLiteral("HighSpeedPass#2026");

    EXPECT_FALSE(db.userExists(username));
    ASSERT_TRUE(db.createUser(username, password, QStringLiteral("engineer")));
    EXPECT_TRUE(db.userExists(username));

    // Authenticate
    EXPECT_TRUE(db.authenticateUser(username, password));
    EXPECT_FALSE(db.authenticateUser(username, QStringLiteral("invalidPass")));

    // Change password
    QString newPassword = QStringLiteral("UpdatedPass#2027");
    ASSERT_TRUE(db.changePassword(username, newPassword));

    // Old password must fail, new password must succeed
    EXPECT_FALSE(db.authenticateUser(username, password));
    EXPECT_TRUE(db.authenticateUser(username, newPassword));
}

TEST_F(DatabaseManagerTest, LogRoundExecution) {
    DatabaseManager db;
    ASSERT_TRUE(db.initLocalDatabase());

    EXPECT_TRUE(db.logRound(1, 84.321));
    EXPECT_TRUE(db.logRound(2, 83.910));
}


TEST_F(DatabaseManagerTest, StrictLoginBlockedWhenDatabaseMissingOrDisconnected) {
    DatabaseManager db;
    // Before initLocalDatabase() creates coffeebreak.db, openExistingDatabase() and authenticateUser() must fail
    EXPECT_FALSE(db.openExistingDatabase(QStringLiteral("non_existent_coffeebreak.db")));
    EXPECT_FALSE(db.isConnected());
    EXPECT_FALSE(db.authenticateUser(QStringLiteral("admin"), QStringLiteral("admin")));

    // Initialize DB, then disconnect: authenticateUser("admin", "admin") must still fail while disconnected
    ASSERT_TRUE(db.initLocalDatabase());
    EXPECT_TRUE(db.authenticateUser(QStringLiteral("admin"), QStringLiteral("admin")));
    db.disconnectDatabase();
    EXPECT_FALSE(db.isConnected());
    EXPECT_FALSE(db.authenticateUser(QStringLiteral("admin"), QStringLiteral("admin")));
}

TEST_F(DatabaseManagerTest, ComprehensiveTelemetrySchemaAndRecording) {
    DatabaseManager db;
    ASSERT_TRUE(db.initLocalDatabase());
    ASSERT_TRUE(db.authenticateUser(QStringLiteral("admin"), QStringLiteral("admin")));

    int sid = db.startSession(
        QStringLiteral("VideoFile"),
        QStringLiteral("D:/Containers/testdata/2026-09-05 06-40-47.remuxed_1.mp4"),
        QStringLiteral("C:/Users/rolan/Documents/EagleEye/eagle_eye_model.pth"),
        QRect(1684, 29, 189, 130),
        QStringLiteral("Auto"));
    EXPECT_GT(sid, 0);

    DatabaseManager::RoundTelemetryRecord rec;
    rec.roundNumber = 112;
    rec.roundLabel = QStringLiteral("112");
    rec.durationSec = 87.0;
    rec.deltaPrevSec = 0.0;
    rec.cumulativeRunSec = 87.0;
    rec.aiConfidence = 0.99;
    rec.digitMode = QStringLiteral("Auto");
    rec.feedMode = QStringLiteral("VideoFile");
    rec.sourceName = QStringLiteral("2026-09-05 06-40-47.remuxed_1.mp4");
    rec.videoPosMs = 87000;
    rec.playbackSpeed = 1.0;
    rec.gpuTempC = 64.0;
    rec.gpuUtilPct = 42.0;
    rec.vramUsedMb = 4120;
    EXPECT_TRUE(db.logRoundDetailed(rec));

    EXPECT_TRUE(db.logGpuSnapshot(
        QStringLiteral("NVIDIA GeForce RTX 3090 Ti"), 64.0, 55.0, 42.0,
        4120, 24564, 112, 60,
        QStringLiteral("192.168.1.100"), QStringLiteral("203.0.113.10")));
    EXPECT_TRUE(db.logSystemEvent(QStringLiteral("AI"), QStringLiteral("Round 112 completed")));

    auto rounds = db.loadRoundRecords();
    ASSERT_EQ(rounds.size(), 1);
    EXPECT_EQ(rounds[0].roundNumber, 112);
    EXPECT_DOUBLE_EQ(rounds[0].durationSec, 87.0);

    QString summary = db.databaseSummary();
    EXPECT_TRUE(summary.contains(QStringLiteral("roundsTable: 1")));
    EXPECT_TRUE(summary.contains(QStringLiteral("gpu_telemetry_log: 1")));
}
