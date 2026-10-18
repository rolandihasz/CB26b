/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "laptimechart.h"
#include "signalchart.h"
#include "videofeedwidget.h"
#include "wavepacketchart.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpenVideo;
    QAction *actionPlayVideo;
    QAction *actionPauseVideo;
    QAction *actionStopVideo;
    QAction *actionLiveCamera;
    QAction *actionSetIteration;
    QAction *actionRunQuery;
    QAction *actionRequestData;
    QAction *actionRefresh;
    QAction *actionDropDbTable;
    QAction *actionClearLog;
    QAction *actionExit;
    QWidget *centralwidget;
    QVBoxLayout *mainVBox;
    QSplitter *vSplitter;
    QSplitter *topRowSplitter;
    QFrame *cardVideoFeed;
    QVBoxLayout *cardVideoLayout;
    QWidget *videoHeaderBar;
    QHBoxLayout *videoHeaderLayout;
    QLabel *videoTitle;
    QSpacerItem *videoHeaderSpacer;
    QPushButton *btnVideoToggle;
    VideoFeedWidget *videoFeed;
    QFrame *cardSignalChart;
    QVBoxLayout *cardSignalLayout;
    QWidget *signalHeaderBar;
    QHBoxLayout *signalHeaderLayout;
    QLabel *signalTitle;
    QSpacerItem *signalHeaderSpacer;
    QLabel *signalBadge;
    SignalChart *signalChart;
    QFrame *cardWavepacketChart;
    QVBoxLayout *cardWaveLayout;
    QWidget *waveHeaderBar;
    QHBoxLayout *waveHeaderLayout;
    QLabel *waveTitle;
    QSpacerItem *waveHeaderSpacer;
    QLabel *waveBadge;
    WavepacketChart *wavepacketChart;
    QSplitter *botRowSplitter;
    QFrame *cardLapTime;
    QVBoxLayout *cardLapLayout;
    QWidget *lapHeaderBar;
    QHBoxLayout *lapHeaderLayout;
    QLabel *lapTitle;
    QSpacerItem *lapHeaderSpacer;
    QLabel *lapBadge;
    LapTimeChart *lapTimeChart;
    QWidget *eventLogContainer;
    QVBoxLayout *eventLogLayout;
    QWidget *logHeaderBar;
    QHBoxLayout *logHeaderLayout;
    QLabel *eventLogHeader;
    QSpacerItem *logHeaderSpacer;
    QLabel *logStreamBadge;
    QPlainTextEdit *eventLogView;
    QWidget *rightPanelContainer;
    QHBoxLayout *rightPanelHBox;
    QTabWidget *dbTabs;
    QWidget *tabSqlite;
    QVBoxLayout *tabSqliteLayout;
    QLabel *tabTitle;
    QHBoxLayout *r1;
    QLineEdit *iterationEdit;
    QPushButton *btnReqData;
    QHBoxLayout *r2;
    QPushButton *btnSetIter;
    QPushButton *btnSave;
    QHBoxLayout *r3;
    QPushButton *btnRunQuery;
    QPushButton *btnPlotData;
    QHBoxLayout *rTelemetry;
    QLabel *altitudeLabel;
    QLabel *positionLabel;
    QHBoxLayout *rProg;
    QLabel *lblProc;
    QProgressBar *progressBar;
    QLabel *warnLbl;
    QHBoxLayout *rWarn1;
    QPushButton *btnDrop1;
    QPushButton *btnDisconn1;
    QHBoxLayout *rWarn2;
    QPushButton *btnDisconn2;
    QPushButton *btnDrop2;
    QSpacerItem *verticalSpacer;
    QWidget *tabPostgreSql;
    QVBoxLayout *tabPgLayout;
    QLabel *tabPgTitle;
    QHBoxLayout *rPg1;
    QLineEdit *iterationEditPg;
    QPushButton *btnReqDataPg;
    QHBoxLayout *rPg2;
    QPushButton *btnSetIterPg;
    QPushButton *btnSavePg;
    QSpacerItem *spacerPg;
    QWidget *tabMySql;
    QVBoxLayout *tabMySqlLayout;
    QLabel *tabMySqlTitle;
    QHBoxLayout *rMySql1;
    QLineEdit *iterationEditMySql;
    QPushButton *btnReqDataMySql;
    QSpacerItem *spacerMySql;
    QGroupBox *connGroup;
    QVBoxLayout *connLayout;
    QLabel *lblIntIp;
    QLineEdit *internalIpEdit;
    QLabel *lblExtIp;
    QLineEdit *externalIpEdit;
    QLabel *lblStatusTitle;
    QLabel *statusLabel;
    QPushButton *btnLiveFeed;
    QPushButton *btnHelpDesk;
    QPushButton *btnAccount;
    QSpacerItem *spacerConn;
    QPushButton *btnExit;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 800);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/icons/database.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setStyleSheet(QString::fromUtf8("/* ===================================================================\n"
"   COFFEEBREAK TELEMETRY - WORLD CLASS MISSION CONTROL THEME\n"
"   =================================================================== */\n"
"\n"
"QMainWindow {\n"
"    background-color: #0b0f19;\n"
"    color: #f8fafc;\n"
"    font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, 'Inter', 'Roboto', sans-serif;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QWidget#centralwidget {\n"
"    background-color: #0b0f19;\n"
"}\n"
"\n"
"/* --- Splitters --- */\n"
"QSplitter::handle {\n"
"    background-color: #1e293b;\n"
"}\n"
"QSplitter::handle:horizontal {\n"
"    width: 3px;\n"
"}\n"
"QSplitter::handle:vertical {\n"
"    height: 3px;\n"
"}\n"
"QSplitter::handle:hover {\n"
"    background-color: #38bdf8;\n"
"}\n"
"\n"
"/* --- Card Containers and Panels --- */\n"
"QFrame[isCard=\"true\"] {\n"
"    background-color: #0f172a;\n"
"    border: 1px solid #1e293b;\n"
"    border-radius: 6px;\n"
"}\n"
"QFrame[isCard=\"true\"]:hover {\n"
"    border:"
                        " 1px solid #334155;\n"
"}\n"
"\n"
"QWidget#eventLogContainer, QWidget#rightPanelContainer {\n"
"    background-color: #0f172a;\n"
"    border: 1px solid #1e293b;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"/* --- Card Headers --- */\n"
"QLabel[isHeader=\"true\"], QLabel#eventLogHeader {\n"
"    color: #38bdf8;\n"
"    font-family: 'Segoe UI', sans-serif;\n"
"    font-size: 10px;\n"
"    font-weight: bold;\n"
"    letter-spacing: 0.8px;\n"
"    padding: 5px 10px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #162238, stop:1 #111a2c);\n"
"    border-bottom: 1px solid #1e293b;\n"
"    border-top-left-radius: 5px;\n"
"    border-top-right-radius: 5px;\n"
"}\n"
"\n"
"QLabel[isBadge=\"true\"] {\n"
"    background-color: #1e293b;\n"
"    color: #94a3b8;\n"
"    font-size: 9px;\n"
"    font-weight: 600;\n"
"    padding: 2px 6px;\n"
"    border-radius: 3px;\n"
"    border: 1px solid #334155;\n"
"}\n"
"\n"
"/* --- Toolbar --- */\n"
"QToolBar#mainToolBar {\n"
"    background: qlineargradient(x1:0, y1:"
                        "0, x2:0, y2:1, stop:0 #151e30, stop:1 #0c121e);\n"
"    border-bottom: 1px solid #1e293b;\n"
"    spacing: 6px;\n"
"    padding: 3px 8px;\n"
"}\n"
"\n"
"QToolButton {\n"
"    background-color: #1e293b;\n"
"    color: #e2e8f0;\n"
"    border: 1px solid #334155;\n"
"    border-radius: 5px;\n"
"    padding: 5px 12px;\n"
"    font-size: 11px;\n"
"    font-weight: 500;\n"
"}\n"
"\n"
"QToolButton:hover {\n"
"    background-color: #27354d;\n"
"    border-color: #38bdf8;\n"
"    color: #38bdf8;\n"
"}\n"
"\n"
"QToolButton:pressed {\n"
"    background-color: #0f172a;\n"
"    border-color: #0284c7;\n"
"    color: #0284c7;\n"
"}\n"
"\n"
"/* --- Push Buttons --- */\n"
"QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #222f44, stop:1 #1a2436);\n"
"    color: #f1f5f9;\n"
"    border: 1px solid #334155;\n"
"    border-radius: 5px;\n"
"    padding: 5px 10px;\n"
"    font-size: 11px;\n"
"    font-weight: 600;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0,"
                        " y2:1, stop:0 #2d3e5a, stop:1 #223048);\n"
"    border-color: #38bdf8;\n"
"    color: #ffffff;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background: #141d2c;\n"
"    border-color: #0284c7;\n"
"    color: #38bdf8;\n"
"}\n"
"\n"
"QPushButton:disabled {\n"
"    background: #111827;\n"
"    border-color: #1f2937;\n"
"    color: #4b5563;\n"
"}\n"
"\n"
"/* Primary Action: Run Query */\n"
"QPushButton#btnRunQuery {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0284c7, stop:1 #0369a1);\n"
"    border: 1px solid #38bdf8;\n"
"    color: #ffffff;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton#btnRunQuery:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0ea5e9, stop:1 #0284c7);\n"
"    border-color: #7dd3fc;\n"
"}\n"
"\n"
"/* Success Action: Save */\n"
"QPushButton#btnSave, QPushButton#btnSavePg {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #059669, stop:1 #047857);\n"
"    border: 1px solid #10b981;\n"
"    color: #ffffff;\n"
"}\n"
"QPushButton#"
                        "btnSave:hover, QPushButton#btnSavePg:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #10b981, stop:1 #059669);\n"
"    border-color: #34d399;\n"
"}\n"
"\n"
"/* Secondary Action: Plot Data */\n"
"QPushButton#btnPlotData {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4f46e5, stop:1 #4338ca);\n"
"    border: 1px solid #818cf8;\n"
"    color: #ffffff;\n"
"}\n"
"QPushButton#btnPlotData:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6366f1, stop:1 #4f46e5);\n"
"}\n"
"\n"
"/* Live Stream Toggle Buttons */\n"
"QPushButton#btnLiveFeed, QPushButton#btnVideoToggle {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #064e3b, stop:1 #065f46);\n"
"    border: 1px solid #10b981;\n"
"    color: #34d399;\n"
"    font-weight: bold;\n"
"    padding: 4px 10px;\n"
"}\n"
"QPushButton#btnLiveFeed:hover, QPushButton#btnVideoToggle:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #047857, stop:1 #065f46);\n"
"    b"
                        "order-color: #34d399;\n"
"    color: #ffffff;\n"
"}\n"
"QPushButton#btnLiveFeed:checked, QPushButton#btnVideoToggle:checked {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #065f46, stop:1 #047857);\n"
"    border: 1px solid #34d399;\n"
"    color: #a7f3d0;\n"
"}\n"
"QPushButton#btnLiveFeed:!checked, QPushButton#btnVideoToggle:!checked {\n"
"    background: #1f2937;\n"
"    border: 1px solid #6b7280;\n"
"    color: #9ca3af;\n"
"}\n"
"\n"
"/* Danger Zone Buttons (Drop Table) */\n"
"QPushButton#btnDrop1, QPushButton#btnDrop2 {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7f1d1d, stop:1 #991b1b);\n"
"    border: 1px solid #ef4444;\n"
"    color: #fee2e2;\n"
"}\n"
"QPushButton#btnDrop1:hover, QPushButton#btnDrop2:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #991b1b, stop:1 #b91c1c);\n"
"    border-color: #f87171;\n"
"}\n"
"\n"
"/* Warning Buttons (Disconnect) */\n"
"QPushButton#btnDisconn1, QPushButton#btnDisconn2 {\n"
"    background: qlinea"
                        "rgradient(x1:0, y1:0, x2:0, y2:1, stop:0 #78350f, stop:1 #92400e);\n"
"    border: 1px solid #f59e0b;\n"
"    color: #fef3c7;\n"
"}\n"
"QPushButton#btnDisconn1:hover, QPushButton#btnDisconn2:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #92400e, stop:1 #b45309);\n"
"    border-color: #fbbf24;\n"
"}\n"
"\n"
"/* Exit Button */\n"
"QPushButton#btnExit {\n"
"    background: #1e293b;\n"
"    border: 1px solid #475569;\n"
"    color: #f87171;\n"
"}\n"
"QPushButton#btnExit:hover {\n"
"    background: #991b1b;\n"
"    border-color: #ef4444;\n"
"    color: #ffffff;\n"
"}\n"
"\n"
"/* --- LineEdits --- */\n"
"QLineEdit {\n"
"    background-color: #0b0f19;\n"
"    border: 1px solid #334155;\n"
"    border-radius: 4px;\n"
"    padding: 4px 8px;\n"
"    color: #f8fafc;\n"
"    font-family: 'Consolas', 'Courier New', monospace;\n"
"    font-size: 11px;\n"
"}\n"
"QLineEdit:focus {\n"
"    border: 1px solid #38bdf8;\n"
"    background-color: #0f172a;\n"
"}\n"
"QLineEdit[readOnly=\"true\"] {\n"
"   "
                        " background-color: #080c14;\n"
"    border-color: #1e293b;\n"
"    color: #38bdf8;\n"
"}\n"
"\n"
"/* --- Labels --- */\n"
"QLabel {\n"
"    color: #cbd5e1;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"/* --- Status Badges --- */\n"
"QLabel#statusLabel {\n"
"    background-color: #064e3b;\n"
"    color: #34d399;\n"
"    border: 1px solid #059669;\n"
"    border-radius: 4px;\n"
"    padding: 4px 8px;\n"
"    font-weight: bold;\n"
"    font-size: 10px;\n"
"    letter-spacing: 0.5px;\n"
"}\n"
"\n"
"QLabel#altitudeLabel, QLabel#positionLabel {\n"
"    background-color: #0b0f19;\n"
"    border: 1px solid #1e293b;\n"
"    border-radius: 4px;\n"
"    padding: 4px 8px;\n"
"    color: #94a3b8;\n"
"    font-family: 'Consolas', monospace;\n"
"    font-size: 10px;\n"
"}\n"
"\n"
"QLabel#warnLbl {\n"
"    color: #f87171;\n"
"    font-weight: bold;\n"
"    font-size: 10px;\n"
"    padding: 4px 6px;\n"
"    background-color: rgba(127, 29, 29, 0.25);\n"
"    border: 1px solid rgba(239, 68, 68, 0.4);\n"
"    border-radius: 4px;\n"
"}"
                        "\n"
"\n"
"/* --- Group Boxes --- */\n"
"QGroupBox {\n"
"    background-color: #0f172a;\n"
"    border: 1px solid #1e293b;\n"
"    border-radius: 6px;\n"
"    margin-top: 18px;\n"
"    padding: 12px 8px 8px 8px;\n"
"    font-size: 10px;\n"
"    font-weight: bold;\n"
"    color: #38bdf8;\n"
"    letter-spacing: 0.5px;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    left: 10px;\n"
"    top: 3px;\n"
"    padding: 0 4px;\n"
"    background-color: #0f172a;\n"
"    color: #38bdf8;\n"
"}\n"
"\n"
"/* --- Tab Widget --- */\n"
"QTabWidget::pane {\n"
"    border: 1px solid #1e293b;\n"
"    background: #0f172a;\n"
"    border-bottom-left-radius: 6px;\n"
"    border-bottom-right-radius: 6px;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background: #111827;\n"
"    border: 1px solid #1e293b;\n"
"    border-bottom: none;\n"
"    border-top-left-radius: 5px;\n"
"    border-top-right-radius: 5px;\n"
"    padding: 5px 12px;\n"
"    margin-right: 2px;\n"
"    font-size: 11px"
                        ";\n"
"    font-weight: 500;\n"
"    color: #94a3b8;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    background: #0f172a;\n"
"    border-color: #38bdf8;\n"
"    border-bottom-color: #0f172a;\n"
"    font-weight: bold;\n"
"    color: #38bdf8;\n"
"}\n"
"\n"
"QTabBar::tab:hover:!selected {\n"
"    background: #1f293d;\n"
"    color: #f1f5f9;\n"
"}\n"
"\n"
"/* --- Progress Bar --- */\n"
"QProgressBar {\n"
"    background-color: #0b0f19;\n"
"    border: 1px solid #1e293b;\n"
"    border-radius: 4px;\n"
"    text-align: center;\n"
"    height: 16px;\n"
"    font-size: 10px;\n"
"    font-weight: bold;\n"
"    color: #ffffff;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0284c7, stop:1 #38bdf8);\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"/* --- Event Log View (Terminal) --- */\n"
"QPlainTextEdit#eventLogView {\n"
"    background-color: #070a12;\n"
"    color: #22c55e;\n"
"    font-family: 'Consolas', 'Cascadia Code', 'Courier New', monospace;\n"
"    font-size: "
                        "9pt;\n"
"    border: 1px solid #1e293b;\n"
"    border-bottom-left-radius: 5px;\n"
"    border-bottom-right-radius: 5px;\n"
"    selection-background-color: #064e3b;\n"
"    selection-color: #34d399;\n"
"    padding: 6px;\n"
"}\n"
"\n"
"/* --- ScrollBars --- */\n"
"QScrollBar:vertical {\n"
"    border: none;\n"
"    background: #070a12;\n"
"    width: 8px;\n"
"    margin: 0px 0px 0px 0px;\n"
"}\n"
"QScrollBar::handle:vertical {\n"
"    background: #334155;\n"
"    min-height: 20px;\n"
"    border-radius: 4px;\n"
"}\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: #475569;\n"
"}\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"    height: 0px;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    border: none;\n"
"    background: #070a12;\n"
"    height: 8px;\n"
"}\n"
"QScrollBar::handle:horizontal {\n"
"    background: #334155;\n"
"    min-width: 20px;\n"
"    border-radius: 4px;\n"
"}\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background: #475569;\n"
"}\n"
"QScrollBar::add-lin"
                        "e:horizontal, QScrollBar::sub-line:horizontal {\n"
"    width: 0px;\n"
"}\n"
"\n"
"/* --- Status Bar --- */\n"
"QStatusBar {\n"
"    background-color: #0b0f19;\n"
"    border-top: 1px solid #1e293b;\n"
"    color: #64748b;\n"
"    font-size: 10px;\n"
"}\n"
""));
        actionOpenVideo = new QAction(MainWindow);
        actionOpenVideo->setObjectName("actionOpenVideo");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/folder-open.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionOpenVideo->setIcon(icon1);
        actionPlayVideo = new QAction(MainWindow);
        actionPlayVideo->setObjectName("actionPlayVideo");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/icons/play.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionPlayVideo->setIcon(icon2);
        actionPauseVideo = new QAction(MainWindow);
        actionPauseVideo->setObjectName("actionPauseVideo");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/icons/pause.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionPauseVideo->setIcon(icon3);
        actionStopVideo = new QAction(MainWindow);
        actionStopVideo->setObjectName("actionStopVideo");
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/icons/stop.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionStopVideo->setIcon(icon4);
        actionLiveCamera = new QAction(MainWindow);
        actionLiveCamera->setObjectName("actionLiveCamera");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/icons/camera.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionLiveCamera->setIcon(icon5);
        actionSetIteration = new QAction(MainWindow);
        actionSetIteration->setObjectName("actionSetIteration");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/icons/cloud.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionSetIteration->setIcon(icon6);
        actionRunQuery = new QAction(MainWindow);
        actionRunQuery->setObjectName("actionRunQuery");
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/icons/rocket.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionRunQuery->setIcon(icon7);
        actionRequestData = new QAction(MainWindow);
        actionRequestData->setObjectName("actionRequestData");
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/icons/bubble.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionRequestData->setIcon(icon8);
        actionRefresh = new QAction(MainWindow);
        actionRefresh->setObjectName("actionRefresh");
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/icons/sync.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionRefresh->setIcon(icon9);
        actionDropDbTable = new QAction(MainWindow);
        actionDropDbTable->setObjectName("actionDropDbTable");
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icons/icons/trash.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionDropDbTable->setIcon(icon10);
        actionClearLog = new QAction(MainWindow);
        actionClearLog->setObjectName("actionClearLog");
        actionClearLog->setIcon(icon10);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/icons/exit.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionExit->setIcon(icon11);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainVBox = new QVBoxLayout(centralwidget);
        mainVBox->setSpacing(6);
        mainVBox->setObjectName("mainVBox");
        mainVBox->setContentsMargins(6, 6, 6, 6);
        vSplitter = new QSplitter(centralwidget);
        vSplitter->setObjectName("vSplitter");
        vSplitter->setOrientation(Qt::Orientation::Vertical);
        vSplitter->setChildrenCollapsible(false);
        topRowSplitter = new QSplitter(vSplitter);
        topRowSplitter->setObjectName("topRowSplitter");
        topRowSplitter->setOrientation(Qt::Orientation::Horizontal);
        topRowSplitter->setChildrenCollapsible(false);
        cardVideoFeed = new QFrame(topRowSplitter);
        cardVideoFeed->setObjectName("cardVideoFeed");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(50);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cardVideoFeed->sizePolicy().hasHeightForWidth());
        cardVideoFeed->setSizePolicy(sizePolicy);
        cardVideoFeed->setMinimumSize(QSize(640, 360));
        cardVideoFeed->setFrameShape(QFrame::Shape::StyledPanel);
        cardVideoFeed->setProperty("isCard", QVariant(true));
        cardVideoLayout = new QVBoxLayout(cardVideoFeed);
        cardVideoLayout->setSpacing(0);
        cardVideoLayout->setObjectName("cardVideoLayout");
        cardVideoLayout->setContentsMargins(0, 0, 0, 0);
        videoHeaderBar = new QWidget(cardVideoFeed);
        videoHeaderBar->setObjectName("videoHeaderBar");
        videoHeaderLayout = new QHBoxLayout(videoHeaderBar);
        videoHeaderLayout->setSpacing(6);
        videoHeaderLayout->setObjectName("videoHeaderLayout");
        videoHeaderLayout->setContentsMargins(8, 4, 8, 4);
        videoTitle = new QLabel(videoHeaderBar);
        videoTitle->setObjectName("videoTitle");
        videoTitle->setProperty("isHeader", QVariant(true));

        videoHeaderLayout->addWidget(videoTitle);

        videoHeaderSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        videoHeaderLayout->addItem(videoHeaderSpacer);

        btnVideoToggle = new QPushButton(videoHeaderBar);
        btnVideoToggle->setObjectName("btnVideoToggle");
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/icons/icons/power-switch.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnVideoToggle->setIcon(icon12);
        btnVideoToggle->setCheckable(true);
        btnVideoToggle->setChecked(true);

        videoHeaderLayout->addWidget(btnVideoToggle);


        cardVideoLayout->addWidget(videoHeaderBar);

        videoFeed = new VideoFeedWidget(cardVideoFeed);
        videoFeed->setObjectName("videoFeed");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(videoFeed->sizePolicy().hasHeightForWidth());
        videoFeed->setSizePolicy(sizePolicy1);

        cardVideoLayout->addWidget(videoFeed);

        topRowSplitter->addWidget(cardVideoFeed);
        cardSignalChart = new QFrame(topRowSplitter);
        cardSignalChart->setObjectName("cardSignalChart");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(25);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(cardSignalChart->sizePolicy().hasHeightForWidth());
        cardSignalChart->setSizePolicy(sizePolicy2);
        cardSignalChart->setMinimumSize(QSize(250, 180));
        cardSignalChart->setFrameShape(QFrame::Shape::StyledPanel);
        cardSignalChart->setProperty("isCard", QVariant(true));
        cardSignalLayout = new QVBoxLayout(cardSignalChart);
        cardSignalLayout->setSpacing(0);
        cardSignalLayout->setObjectName("cardSignalLayout");
        cardSignalLayout->setContentsMargins(0, 0, 0, 0);
        signalHeaderBar = new QWidget(cardSignalChart);
        signalHeaderBar->setObjectName("signalHeaderBar");
        signalHeaderLayout = new QHBoxLayout(signalHeaderBar);
        signalHeaderLayout->setSpacing(6);
        signalHeaderLayout->setObjectName("signalHeaderLayout");
        signalHeaderLayout->setContentsMargins(8, 4, 8, 4);
        signalTitle = new QLabel(signalHeaderBar);
        signalTitle->setObjectName("signalTitle");
        signalTitle->setProperty("isHeader", QVariant(true));

        signalHeaderLayout->addWidget(signalTitle);

        signalHeaderSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        signalHeaderLayout->addItem(signalHeaderSpacer);

        signalBadge = new QLabel(signalHeaderBar);
        signalBadge->setObjectName("signalBadge");
        signalBadge->setProperty("isBadge", QVariant(true));

        signalHeaderLayout->addWidget(signalBadge);


        cardSignalLayout->addWidget(signalHeaderBar);

        signalChart = new SignalChart(cardSignalChart);
        signalChart->setObjectName("signalChart");
        sizePolicy1.setHeightForWidth(signalChart->sizePolicy().hasHeightForWidth());
        signalChart->setSizePolicy(sizePolicy1);

        cardSignalLayout->addWidget(signalChart);

        topRowSplitter->addWidget(cardSignalChart);
        cardWavepacketChart = new QFrame(topRowSplitter);
        cardWavepacketChart->setObjectName("cardWavepacketChart");
        sizePolicy2.setHeightForWidth(cardWavepacketChart->sizePolicy().hasHeightForWidth());
        cardWavepacketChart->setSizePolicy(sizePolicy2);
        cardWavepacketChart->setMinimumSize(QSize(250, 180));
        cardWavepacketChart->setFrameShape(QFrame::Shape::StyledPanel);
        cardWavepacketChart->setProperty("isCard", QVariant(true));
        cardWaveLayout = new QVBoxLayout(cardWavepacketChart);
        cardWaveLayout->setSpacing(0);
        cardWaveLayout->setObjectName("cardWaveLayout");
        cardWaveLayout->setContentsMargins(0, 0, 0, 0);
        waveHeaderBar = new QWidget(cardWavepacketChart);
        waveHeaderBar->setObjectName("waveHeaderBar");
        waveHeaderLayout = new QHBoxLayout(waveHeaderBar);
        waveHeaderLayout->setSpacing(6);
        waveHeaderLayout->setObjectName("waveHeaderLayout");
        waveHeaderLayout->setContentsMargins(8, 4, 8, 4);
        waveTitle = new QLabel(waveHeaderBar);
        waveTitle->setObjectName("waveTitle");
        waveTitle->setProperty("isHeader", QVariant(true));

        waveHeaderLayout->addWidget(waveTitle);

        waveHeaderSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        waveHeaderLayout->addItem(waveHeaderSpacer);

        waveBadge = new QLabel(waveHeaderBar);
        waveBadge->setObjectName("waveBadge");
        waveBadge->setProperty("isBadge", QVariant(true));

        waveHeaderLayout->addWidget(waveBadge);


        cardWaveLayout->addWidget(waveHeaderBar);

        wavepacketChart = new WavepacketChart(cardWavepacketChart);
        wavepacketChart->setObjectName("wavepacketChart");
        sizePolicy1.setHeightForWidth(wavepacketChart->sizePolicy().hasHeightForWidth());
        wavepacketChart->setSizePolicy(sizePolicy1);

        cardWaveLayout->addWidget(wavepacketChart);

        topRowSplitter->addWidget(cardWavepacketChart);
        vSplitter->addWidget(topRowSplitter);
        botRowSplitter = new QSplitter(vSplitter);
        botRowSplitter->setObjectName("botRowSplitter");
        botRowSplitter->setOrientation(Qt::Orientation::Horizontal);
        botRowSplitter->setChildrenCollapsible(false);
        cardLapTime = new QFrame(botRowSplitter);
        cardLapTime->setObjectName("cardLapTime");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(35);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(cardLapTime->sizePolicy().hasHeightForWidth());
        cardLapTime->setSizePolicy(sizePolicy3);
        cardLapTime->setMinimumSize(QSize(250, 180));
        cardLapTime->setFrameShape(QFrame::Shape::StyledPanel);
        cardLapTime->setProperty("isCard", QVariant(true));
        cardLapLayout = new QVBoxLayout(cardLapTime);
        cardLapLayout->setSpacing(0);
        cardLapLayout->setObjectName("cardLapLayout");
        cardLapLayout->setContentsMargins(0, 0, 0, 0);
        lapHeaderBar = new QWidget(cardLapTime);
        lapHeaderBar->setObjectName("lapHeaderBar");
        lapHeaderLayout = new QHBoxLayout(lapHeaderBar);
        lapHeaderLayout->setSpacing(6);
        lapHeaderLayout->setObjectName("lapHeaderLayout");
        lapHeaderLayout->setContentsMargins(8, 4, 8, 4);
        lapTitle = new QLabel(lapHeaderBar);
        lapTitle->setObjectName("lapTitle");
        lapTitle->setProperty("isHeader", QVariant(true));

        lapHeaderLayout->addWidget(lapTitle);

        lapHeaderSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        lapHeaderLayout->addItem(lapHeaderSpacer);

        lapBadge = new QLabel(lapHeaderBar);
        lapBadge->setObjectName("lapBadge");
        lapBadge->setProperty("isBadge", QVariant(true));

        lapHeaderLayout->addWidget(lapBadge);


        cardLapLayout->addWidget(lapHeaderBar);

        lapTimeChart = new LapTimeChart(cardLapTime);
        lapTimeChart->setObjectName("lapTimeChart");
        sizePolicy1.setHeightForWidth(lapTimeChart->sizePolicy().hasHeightForWidth());
        lapTimeChart->setSizePolicy(sizePolicy1);

        cardLapLayout->addWidget(lapTimeChart);

        botRowSplitter->addWidget(cardLapTime);
        eventLogContainer = new QWidget(botRowSplitter);
        eventLogContainer->setObjectName("eventLogContainer");
        sizePolicy3.setHeightForWidth(eventLogContainer->sizePolicy().hasHeightForWidth());
        eventLogContainer->setSizePolicy(sizePolicy3);
        eventLogLayout = new QVBoxLayout(eventLogContainer);
        eventLogLayout->setSpacing(0);
        eventLogLayout->setObjectName("eventLogLayout");
        eventLogLayout->setContentsMargins(0, 0, 0, 0);
        logHeaderBar = new QWidget(eventLogContainer);
        logHeaderBar->setObjectName("logHeaderBar");
        logHeaderLayout = new QHBoxLayout(logHeaderBar);
        logHeaderLayout->setSpacing(6);
        logHeaderLayout->setObjectName("logHeaderLayout");
        logHeaderLayout->setContentsMargins(8, 4, 8, 4);
        eventLogHeader = new QLabel(logHeaderBar);
        eventLogHeader->setObjectName("eventLogHeader");
        eventLogHeader->setProperty("isHeader", QVariant(true));

        logHeaderLayout->addWidget(eventLogHeader);

        logHeaderSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        logHeaderLayout->addItem(logHeaderSpacer);

        logStreamBadge = new QLabel(logHeaderBar);
        logStreamBadge->setObjectName("logStreamBadge");
        logStreamBadge->setProperty("isBadge", QVariant(true));

        logHeaderLayout->addWidget(logStreamBadge);


        eventLogLayout->addWidget(logHeaderBar);

        eventLogView = new QPlainTextEdit(eventLogContainer);
        eventLogView->setObjectName("eventLogView");
        eventLogView->setReadOnly(true);

        eventLogLayout->addWidget(eventLogView);

        botRowSplitter->addWidget(eventLogContainer);
        rightPanelContainer = new QWidget(botRowSplitter);
        rightPanelContainer->setObjectName("rightPanelContainer");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy4.setHorizontalStretch(30);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(rightPanelContainer->sizePolicy().hasHeightForWidth());
        rightPanelContainer->setSizePolicy(sizePolicy4);
        rightPanelHBox = new QHBoxLayout(rightPanelContainer);
        rightPanelHBox->setSpacing(6);
        rightPanelHBox->setObjectName("rightPanelHBox");
        rightPanelHBox->setContentsMargins(4, 4, 4, 4);
        dbTabs = new QTabWidget(rightPanelContainer);
        dbTabs->setObjectName("dbTabs");
        tabSqlite = new QWidget();
        tabSqlite->setObjectName("tabSqlite");
        tabSqliteLayout = new QVBoxLayout(tabSqlite);
        tabSqliteLayout->setSpacing(5);
        tabSqliteLayout->setObjectName("tabSqliteLayout");
        tabSqliteLayout->setContentsMargins(8, 8, 8, 8);
        tabTitle = new QLabel(tabSqlite);
        tabTitle->setObjectName("tabTitle");
        tabTitle->setStyleSheet(QString::fromUtf8("font-size: 11px; font-weight: bold; color: #38bdf8;"));

        tabSqliteLayout->addWidget(tabTitle);

        r1 = new QHBoxLayout();
        r1->setSpacing(4);
        r1->setObjectName("r1");
        iterationEdit = new QLineEdit(tabSqlite);
        iterationEdit->setObjectName("iterationEdit");

        r1->addWidget(iterationEdit);

        btnReqData = new QPushButton(tabSqlite);
        btnReqData->setObjectName("btnReqData");
        btnReqData->setIcon(icon8);

        r1->addWidget(btnReqData);


        tabSqliteLayout->addLayout(r1);

        r2 = new QHBoxLayout();
        r2->setSpacing(4);
        r2->setObjectName("r2");
        btnSetIter = new QPushButton(tabSqlite);
        btnSetIter->setObjectName("btnSetIter");
        btnSetIter->setIcon(icon6);

        r2->addWidget(btnSetIter);

        btnSave = new QPushButton(tabSqlite);
        btnSave->setObjectName("btnSave");
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/icons/icons/download.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnSave->setIcon(icon13);

        r2->addWidget(btnSave);


        tabSqliteLayout->addLayout(r2);

        r3 = new QHBoxLayout();
        r3->setSpacing(4);
        r3->setObjectName("r3");
        btnRunQuery = new QPushButton(tabSqlite);
        btnRunQuery->setObjectName("btnRunQuery");
        btnRunQuery->setIcon(icon7);

        r3->addWidget(btnRunQuery);

        btnPlotData = new QPushButton(tabSqlite);
        btnPlotData->setObjectName("btnPlotData");
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/icons/icons/chart-bars.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnPlotData->setIcon(icon14);

        r3->addWidget(btnPlotData);


        tabSqliteLayout->addLayout(r3);

        rTelemetry = new QHBoxLayout();
        rTelemetry->setSpacing(4);
        rTelemetry->setObjectName("rTelemetry");
        altitudeLabel = new QLabel(tabSqlite);
        altitudeLabel->setObjectName("altitudeLabel");
        altitudeLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        rTelemetry->addWidget(altitudeLabel);

        positionLabel = new QLabel(tabSqlite);
        positionLabel->setObjectName("positionLabel");
        positionLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        rTelemetry->addWidget(positionLabel);


        tabSqliteLayout->addLayout(rTelemetry);

        rProg = new QHBoxLayout();
        rProg->setSpacing(4);
        rProg->setObjectName("rProg");
        lblProc = new QLabel(tabSqlite);
        lblProc->setObjectName("lblProc");
        lblProc->setStyleSheet(QString::fromUtf8("font-size: 10px; color: #94a3b8;"));

        rProg->addWidget(lblProc);

        progressBar = new QProgressBar(tabSqlite);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);
        progressBar->setTextVisible(true);

        rProg->addWidget(progressBar);


        tabSqliteLayout->addLayout(rProg);

        warnLbl = new QLabel(tabSqlite);
        warnLbl->setObjectName("warnLbl");
        warnLbl->setAlignment(Qt::AlignmentFlag::AlignCenter);

        tabSqliteLayout->addWidget(warnLbl);

        rWarn1 = new QHBoxLayout();
        rWarn1->setSpacing(4);
        rWarn1->setObjectName("rWarn1");
        btnDrop1 = new QPushButton(tabSqlite);
        btnDrop1->setObjectName("btnDrop1");
        btnDrop1->setIcon(icon10);

        rWarn1->addWidget(btnDrop1);

        btnDisconn1 = new QPushButton(tabSqlite);
        btnDisconn1->setObjectName("btnDisconn1");
        btnDisconn1->setIcon(icon12);

        rWarn1->addWidget(btnDisconn1);


        tabSqliteLayout->addLayout(rWarn1);

        rWarn2 = new QHBoxLayout();
        rWarn2->setSpacing(4);
        rWarn2->setObjectName("rWarn2");
        btnDisconn2 = new QPushButton(tabSqlite);
        btnDisconn2->setObjectName("btnDisconn2");

        rWarn2->addWidget(btnDisconn2);

        btnDrop2 = new QPushButton(tabSqlite);
        btnDrop2->setObjectName("btnDrop2");
        btnDrop2->setIcon(icon10);

        rWarn2->addWidget(btnDrop2);


        tabSqliteLayout->addLayout(rWarn2);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        tabSqliteLayout->addItem(verticalSpacer);

        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/icons/icons/sqlite_icon.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        dbTabs->addTab(tabSqlite, icon15, QString());
        tabPostgreSql = new QWidget();
        tabPostgreSql->setObjectName("tabPostgreSql");
        tabPgLayout = new QVBoxLayout(tabPostgreSql);
        tabPgLayout->setSpacing(5);
        tabPgLayout->setObjectName("tabPgLayout");
        tabPgLayout->setContentsMargins(8, 8, 8, 8);
        tabPgTitle = new QLabel(tabPostgreSql);
        tabPgTitle->setObjectName("tabPgTitle");
        tabPgTitle->setStyleSheet(QString::fromUtf8("font-size: 11px; font-weight: bold; color: #38bdf8;"));

        tabPgLayout->addWidget(tabPgTitle);

        rPg1 = new QHBoxLayout();
        rPg1->setSpacing(4);
        rPg1->setObjectName("rPg1");
        iterationEditPg = new QLineEdit(tabPostgreSql);
        iterationEditPg->setObjectName("iterationEditPg");

        rPg1->addWidget(iterationEditPg);

        btnReqDataPg = new QPushButton(tabPostgreSql);
        btnReqDataPg->setObjectName("btnReqDataPg");
        btnReqDataPg->setIcon(icon8);

        rPg1->addWidget(btnReqDataPg);


        tabPgLayout->addLayout(rPg1);

        rPg2 = new QHBoxLayout();
        rPg2->setSpacing(4);
        rPg2->setObjectName("rPg2");
        btnSetIterPg = new QPushButton(tabPostgreSql);
        btnSetIterPg->setObjectName("btnSetIterPg");
        btnSetIterPg->setIcon(icon6);

        rPg2->addWidget(btnSetIterPg);

        btnSavePg = new QPushButton(tabPostgreSql);
        btnSavePg->setObjectName("btnSavePg");
        btnSavePg->setIcon(icon13);

        rPg2->addWidget(btnSavePg);


        tabPgLayout->addLayout(rPg2);

        spacerPg = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        tabPgLayout->addItem(spacerPg);

        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/icons/icons/Postgresql.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        dbTabs->addTab(tabPostgreSql, icon16, QString());
        tabMySql = new QWidget();
        tabMySql->setObjectName("tabMySql");
        tabMySqlLayout = new QVBoxLayout(tabMySql);
        tabMySqlLayout->setSpacing(5);
        tabMySqlLayout->setObjectName("tabMySqlLayout");
        tabMySqlLayout->setContentsMargins(8, 8, 8, 8);
        tabMySqlTitle = new QLabel(tabMySql);
        tabMySqlTitle->setObjectName("tabMySqlTitle");
        tabMySqlTitle->setStyleSheet(QString::fromUtf8("font-size: 11px; font-weight: bold; color: #38bdf8;"));

        tabMySqlLayout->addWidget(tabMySqlTitle);

        rMySql1 = new QHBoxLayout();
        rMySql1->setSpacing(4);
        rMySql1->setObjectName("rMySql1");
        iterationEditMySql = new QLineEdit(tabMySql);
        iterationEditMySql->setObjectName("iterationEditMySql");

        rMySql1->addWidget(iterationEditMySql);

        btnReqDataMySql = new QPushButton(tabMySql);
        btnReqDataMySql->setObjectName("btnReqDataMySql");
        btnReqDataMySql->setIcon(icon8);

        rMySql1->addWidget(btnReqDataMySql);


        tabMySqlLayout->addLayout(rMySql1);

        spacerMySql = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        tabMySqlLayout->addItem(spacerMySql);

        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/icons/icons/Mysql_app.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        dbTabs->addTab(tabMySql, icon17, QString());

        rightPanelHBox->addWidget(dbTabs);

        connGroup = new QGroupBox(rightPanelContainer);
        connGroup->setObjectName("connGroup");
        connLayout = new QVBoxLayout(connGroup);
        connLayout->setSpacing(4);
        connLayout->setObjectName("connLayout");
        connLayout->setContentsMargins(8, 8, 8, 8);
        lblIntIp = new QLabel(connGroup);
        lblIntIp->setObjectName("lblIntIp");
        lblIntIp->setStyleSheet(QString::fromUtf8("font-size: 10px; color: #94a3b8;"));

        connLayout->addWidget(lblIntIp);

        internalIpEdit = new QLineEdit(connGroup);
        internalIpEdit->setObjectName("internalIpEdit");
        internalIpEdit->setReadOnly(true);

        connLayout->addWidget(internalIpEdit);

        lblExtIp = new QLabel(connGroup);
        lblExtIp->setObjectName("lblExtIp");
        lblExtIp->setStyleSheet(QString::fromUtf8("font-size: 10px; color: #94a3b8;"));

        connLayout->addWidget(lblExtIp);

        externalIpEdit = new QLineEdit(connGroup);
        externalIpEdit->setObjectName("externalIpEdit");
        externalIpEdit->setReadOnly(true);

        connLayout->addWidget(externalIpEdit);

        lblStatusTitle = new QLabel(connGroup);
        lblStatusTitle->setObjectName("lblStatusTitle");
        lblStatusTitle->setStyleSheet(QString::fromUtf8("font-size: 10px; color: #94a3b8;"));

        connLayout->addWidget(lblStatusTitle);

        statusLabel = new QLabel(connGroup);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        connLayout->addWidget(statusLabel);

        btnLiveFeed = new QPushButton(connGroup);
        btnLiveFeed->setObjectName("btnLiveFeed");
        btnLiveFeed->setIcon(icon12);
        btnLiveFeed->setCheckable(true);
        btnLiveFeed->setChecked(true);

        connLayout->addWidget(btnLiveFeed);

        btnHelpDesk = new QPushButton(connGroup);
        btnHelpDesk->setObjectName("btnHelpDesk");
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/icons/icons/question-circle.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnHelpDesk->setIcon(icon18);

        connLayout->addWidget(btnHelpDesk);

        btnAccount = new QPushButton(connGroup);
        btnAccount->setObjectName("btnAccount");
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/icons/icons/lock.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnAccount->setIcon(icon19);

        connLayout->addWidget(btnAccount);

        spacerConn = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        connLayout->addItem(spacerConn);

        btnExit = new QPushButton(connGroup);
        btnExit->setObjectName("btnExit");
        btnExit->setIcon(icon11);

        connLayout->addWidget(btnExit);


        rightPanelHBox->addWidget(connGroup);

        botRowSplitter->addWidget(rightPanelContainer);
        vSplitter->addWidget(botRowSplitter);

        mainVBox->addWidget(vSplitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1280, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName("mainToolBar");
        mainToolBar->setMovable(false);
        mainToolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
        mainToolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);

        mainToolBar->addAction(actionOpenVideo);
        mainToolBar->addAction(actionPlayVideo);
        mainToolBar->addAction(actionPauseVideo);
        mainToolBar->addAction(actionStopVideo);
        mainToolBar->addAction(actionLiveCamera);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionClearLog);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionExit);

        retranslateUi(MainWindow);

        dbTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "CoffeeBreak V1.0 Telemetry Workstation", nullptr));
        actionOpenVideo->setText(QCoreApplication::translate("MainWindow", "Open Video", nullptr));
#if QT_CONFIG(tooltip)
        actionOpenVideo->setToolTip(QCoreApplication::translate("MainWindow", "Open pre-recorded gameplay video file", nullptr));
#endif // QT_CONFIG(tooltip)
        actionPlayVideo->setText(QCoreApplication::translate("MainWindow", "Play", nullptr));
#if QT_CONFIG(tooltip)
        actionPlayVideo->setToolTip(QCoreApplication::translate("MainWindow", "Play video & analyse gameplay run", nullptr));
#endif // QT_CONFIG(tooltip)
        actionPauseVideo->setText(QCoreApplication::translate("MainWindow", "Pause", nullptr));
#if QT_CONFIG(tooltip)
        actionPauseVideo->setToolTip(QCoreApplication::translate("MainWindow", "Pause video playback", nullptr));
#endif // QT_CONFIG(tooltip)
        actionStopVideo->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
#if QT_CONFIG(tooltip)
        actionStopVideo->setToolTip(QCoreApplication::translate("MainWindow", "Stop playback and reset to start", nullptr));
#endif // QT_CONFIG(tooltip)
        actionLiveCamera->setText(QCoreApplication::translate("MainWindow", "Live Stream", nullptr));
#if QT_CONFIG(tooltip)
        actionLiveCamera->setToolTip(QCoreApplication::translate("MainWindow", "Switch back to live OBS Virtual Camera", nullptr));
#endif // QT_CONFIG(tooltip)
        actionSetIteration->setText(QCoreApplication::translate("MainWindow", "Set Iteration", nullptr));
#if QT_CONFIG(tooltip)
        actionSetIteration->setToolTip(QCoreApplication::translate("MainWindow", "Apply query iteration configuration", nullptr));
#endif // QT_CONFIG(tooltip)
        actionRunQuery->setText(QCoreApplication::translate("MainWindow", "Run Query", nullptr));
#if QT_CONFIG(tooltip)
        actionRunQuery->setToolTip(QCoreApplication::translate("MainWindow", "Generate sample query telemetry dataset", nullptr));
#endif // QT_CONFIG(tooltip)
        actionRequestData->setText(QCoreApplication::translate("MainWindow", "Request Data", nullptr));
#if QT_CONFIG(tooltip)
        actionRequestData->setToolTip(QCoreApplication::translate("MainWindow", "Request latest dataset from database", nullptr));
#endif // QT_CONFIG(tooltip)
        actionRefresh->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
#if QT_CONFIG(tooltip)
        actionRefresh->setToolTip(QCoreApplication::translate("MainWindow", "Refresh camera stream and system diagnostics", nullptr));
#endif // QT_CONFIG(tooltip)
        actionDropDbTable->setText(QCoreApplication::translate("MainWindow", "Drop Table", nullptr));
#if QT_CONFIG(tooltip)
        actionDropDbTable->setToolTip(QCoreApplication::translate("MainWindow", "Drop active telemetry table (requires confirmation)", nullptr));
#endif // QT_CONFIG(tooltip)
        actionClearLog->setText(QCoreApplication::translate("MainWindow", "Clear Log", nullptr));
#if QT_CONFIG(tooltip)
        actionClearLog->setToolTip(QCoreApplication::translate("MainWindow", "Clear terminal event log output", nullptr));
#endif // QT_CONFIG(tooltip)
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
#if QT_CONFIG(tooltip)
        actionExit->setToolTip(QCoreApplication::translate("MainWindow", "Quit CoffeeBreak Telemetry", nullptr));
#endif // QT_CONFIG(tooltip)
        videoTitle->setText(QCoreApplication::translate("MainWindow", "LIVE VIDEO STREAM \342\200\242 OBS / EAGLEEYE AI DETECTION", nullptr));
#if QT_CONFIG(tooltip)
        btnVideoToggle->setToolTip(QCoreApplication::translate("MainWindow", "Toggle Live Video Stream On / Off", nullptr));
#endif // QT_CONFIG(tooltip)
        btnVideoToggle->setText(QCoreApplication::translate("MainWindow", "STREAM ON", nullptr));
        signalTitle->setText(QCoreApplication::translate("MainWindow", "GPU & SENSOR TELEMETRY", nullptr));
        signalBadge->setText(QCoreApplication::translate("MainWindow", "CH1: UTIL | CH2: TEMP", nullptr));
        waveTitle->setText(QCoreApplication::translate("MainWindow", "WAVEPACKET FREQUENCY", nullptr));
        waveBadge->setText(QCoreApplication::translate("MainWindow", "30 FPS REALTIME", nullptr));
        lapTitle->setText(QCoreApplication::translate("MainWindow", "LAP TIME PERFORMANCE EXPANSION", nullptr));
        lapBadge->setText(QCoreApplication::translate("MainWindow", "HISTORICAL ANALYSIS", nullptr));
        eventLogHeader->setText(QCoreApplication::translate("MainWindow", "DIAGNOSTIC & AUDIT EVENT LOG", nullptr));
        logStreamBadge->setText(QCoreApplication::translate("MainWindow", "\342\227\217 LIVE STREAMING", nullptr));
        tabTitle->setText(QCoreApplication::translate("MainWindow", "SQLite Database Management", nullptr));
#if QT_CONFIG(tooltip)
        iterationEdit->setToolTip(QCoreApplication::translate("MainWindow", "Query Iteration Batch Count", nullptr));
#endif // QT_CONFIG(tooltip)
        iterationEdit->setText(QCoreApplication::translate("MainWindow", "8", nullptr));
#if QT_CONFIG(tooltip)
        btnReqData->setToolTip(QCoreApplication::translate("MainWindow", "Request Table Data", nullptr));
#endif // QT_CONFIG(tooltip)
        btnReqData->setText(QCoreApplication::translate("MainWindow", "Request Data", nullptr));
#if QT_CONFIG(tooltip)
        btnSetIter->setToolTip(QCoreApplication::translate("MainWindow", "Set Iteration Parameter", nullptr));
#endif // QT_CONFIG(tooltip)
        btnSetIter->setText(QCoreApplication::translate("MainWindow", "Set Iteration", nullptr));
#if QT_CONFIG(tooltip)
        btnSave->setToolTip(QCoreApplication::translate("MainWindow", "Save Snapshot & Config", nullptr));
#endif // QT_CONFIG(tooltip)
        btnSave->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
#if QT_CONFIG(tooltip)
        btnRunQuery->setToolTip(QCoreApplication::translate("MainWindow", "Execute Database Query", nullptr));
#endif // QT_CONFIG(tooltip)
        btnRunQuery->setText(QCoreApplication::translate("MainWindow", "Run Query", nullptr));
#if QT_CONFIG(tooltip)
        btnPlotData->setToolTip(QCoreApplication::translate("MainWindow", "Plot Query Results", nullptr));
#endif // QT_CONFIG(tooltip)
        btnPlotData->setText(QCoreApplication::translate("MainWindow", "Plot Data", nullptr));
        altitudeLabel->setText(QCoreApplication::translate("MainWindow", "Alt: 124 m", nullptr));
        positionLabel->setText(QCoreApplication::translate("MainWindow", "Sec A - Lobby", nullptr));
        lblProc->setText(QCoreApplication::translate("MainWindow", "Progress:", nullptr));
        warnLbl->setText(QCoreApplication::translate("MainWindow", "CAUTION: Irreversible Database Actions", nullptr));
        btnDrop1->setText(QCoreApplication::translate("MainWindow", "Drop DB Table", nullptr));
        btnDisconn1->setText(QCoreApplication::translate("MainWindow", "Disconnect", nullptr));
        btnDisconn2->setText(QCoreApplication::translate("MainWindow", "Disconnect", nullptr));
        btnDrop2->setText(QCoreApplication::translate("MainWindow", "Drop Table", nullptr));
        dbTabs->setTabText(dbTabs->indexOf(tabSqlite), QCoreApplication::translate("MainWindow", "SQLite Local", nullptr));
        tabPgTitle->setText(QCoreApplication::translate("MainWindow", "PostgreSQL Database Management", nullptr));
        iterationEditPg->setText(QCoreApplication::translate("MainWindow", "8", nullptr));
        btnReqDataPg->setText(QCoreApplication::translate("MainWindow", "Request Data", nullptr));
        btnSetIterPg->setText(QCoreApplication::translate("MainWindow", "Set Iteration", nullptr));
        btnSavePg->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        dbTabs->setTabText(dbTabs->indexOf(tabPostgreSql), QCoreApplication::translate("MainWindow", "PostgreSQL", nullptr));
        tabMySqlTitle->setText(QCoreApplication::translate("MainWindow", "MySQL Database Management", nullptr));
        iterationEditMySql->setText(QCoreApplication::translate("MainWindow", "8", nullptr));
        btnReqDataMySql->setText(QCoreApplication::translate("MainWindow", "Request Data", nullptr));
        dbTabs->setTabText(dbTabs->indexOf(tabMySql), QCoreApplication::translate("MainWindow", "MySQL", nullptr));
        connGroup->setTitle(QCoreApplication::translate("MainWindow", "SYSTEM CONNECTION", nullptr));
        lblIntIp->setText(QCoreApplication::translate("MainWindow", "Client Internal IP", nullptr));
        internalIpEdit->setText(QCoreApplication::translate("MainWindow", "127.0.0.1", nullptr));
        lblExtIp->setText(QCoreApplication::translate("MainWindow", "Client External IP", nullptr));
        externalIpEdit->setText(QCoreApplication::translate("MainWindow", "192.168.10.192", nullptr));
        lblStatusTitle->setText(QCoreApplication::translate("MainWindow", "System Status", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "\342\227\217 ONLINE", nullptr));
#if QT_CONFIG(tooltip)
        btnLiveFeed->setToolTip(QCoreApplication::translate("MainWindow", "Toggle Live Video Feed Active / Paused", nullptr));
#endif // QT_CONFIG(tooltip)
        btnLiveFeed->setText(QCoreApplication::translate("MainWindow", "Live Stream: ON", nullptr));
        btnHelpDesk->setText(QCoreApplication::translate("MainWindow", "Online HelpDesk", nullptr));
        btnAccount->setText(QCoreApplication::translate("MainWindow", "My Account", nullptr));
        btnExit->setText(QCoreApplication::translate("MainWindow", "Exit Application", nullptr));
        mainToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "MainToolbar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
