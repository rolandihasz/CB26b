TEMPLATE = app
TARGET = cb26
QT += core gui widgets multimedia multimediawidgets charts sql network concurrent
CONFIG += c++23
INCLUDEPATH += .

HEADERS += mainwindow.h \
           eagleeye_classifier.h \
           laptimechart.h \
           signalchart.h \
           wavepacketchart.h \
           videofeedwidget.h \
           telemetrymanager.h \
           databasemanager.h \
           logindialog.h

FORMS += mainwindow.ui \
         logindialog.ui

SOURCES += main.cpp \
           mainwindow.cpp \
           eagleeye_classifier.cpp \
           laptimechart.cpp \
           signalchart.cpp \
           wavepacketchart.cpp \
           videofeedwidget.cpp \
           telemetrymanager.cpp \
           databasemanager.cpp \
           logindialog.cpp

win32: LIBS += -lbcrypt

RESOURCES += icons.qrc
