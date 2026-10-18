#include <gtest/gtest.h>
#include <QGuiApplication>
#include <QDir>
#include <QCoreApplication>

int main(int argc, char **argv)
{
    // Use offscreen QPA so tests run seamlessly in headless/CI environments
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));
    }

    QGuiApplication app(argc, argv);

    // Ensure working directory is set to project root or accessible directory
    // so tests can locate models like eagle_eye_weights.bin
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
