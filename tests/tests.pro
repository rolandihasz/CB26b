TEMPLATE = app
TARGET = cb26_tests
CONFIG += console c++23 testcase
CONFIG -= app_bundle

QT += core gui sql network testlib

INCLUDEPATH += $$PWD/..

# Google Test & Google Mock integration
include($$PWD/googletest.pri)

# Production headers under test
HEADERS += \
    $$PWD/../databasemanager.h \
    $$PWD/../eagleeye_classifier.h \
    $$PWD/../telemetrymanager.h

# Production sources under test
SOURCES += \
    $$PWD/../databasemanager.cpp \
    $$PWD/../eagleeye_classifier.cpp \
    $$PWD/../telemetrymanager.cpp

# Test suites
SOURCES += \
    $$PWD/main_test.cpp \
    $$PWD/test_databasemanager.cpp \
    $$PWD/test_eagleeye_classifier.cpp \
    $$PWD/test_telemetrymanager.cpp

# Windows dependencies (BCrypt for AES-256 / SHA-256 in DatabaseManager)
win32: LIBS += -lbcrypt
