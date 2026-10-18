#include <gtest/gtest.h>
#include "telemetrymanager.h"
#include <QHostAddress>
#include <QSignalSpy>

class TelemetryManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

// ----------------------------------------------------------------------------
// GpuStats Struct Tests
// ----------------------------------------------------------------------------

TEST_F(TelemetryManagerTest, GpuStatsDefaultValues) {
    GpuStats stats;
    EXPECT_TRUE(stats.name.isEmpty());
    EXPECT_EQ(stats.temperatureC, 0);
    EXPECT_EQ(stats.fanSpeedPercent, 0);
    EXPECT_EQ(stats.gpuUtilPercent, 0);
    EXPECT_EQ(stats.memUsedMb, 0);
    EXPECT_EQ(stats.memTotalMb, 0);
}

// ----------------------------------------------------------------------------
// TelemetryManager Network & Diagnostics Tests
// ----------------------------------------------------------------------------

TEST_F(TelemetryManagerTest, InternalIpResolution) {
    TelemetryManager mgr;
    QSignalSpy spy(&mgr, &TelemetryManager::ipUpdated);

    mgr.resolveInternalIp();

    QString internalIp = mgr.internalIp();
    EXPECT_FALSE(internalIp.isEmpty());

    // Verify it is a valid IPv4 address
    QHostAddress addr(internalIp);
    EXPECT_FALSE(addr.isNull());
    EXPECT_EQ(addr.protocol(), QAbstractSocket::IPv4Protocol);

    // Verify signal was emitted
    EXPECT_GE(spy.count(), 1);
}

TEST_F(TelemetryManagerTest, ScreenDiagnosticsExecution) {
    TelemetryManager mgr;
    QStringList diag = mgr.screenDiagnostics();

    // In a GUI or offscreen environment, diagnostics list should be returned safely
    for (const QString &entry : diag) {
        EXPECT_FALSE(entry.isEmpty());
    }
}
