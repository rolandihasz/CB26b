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
    QAction *actionSetIteration;
    QAction *actionRunQuery;
    QAction *actionRequestData;
    QAction *actionRefresh;
    QAction *actionDropDbTable;
    QAction *actionClearLog;
    QAction *actionExit;
    QAction *actionOpenVideo;
    QAction *actionPlayVideo;
    QAction *actionPauseVideo;
    QAction *actionStopVideo;
    QAction *actionLiveCamera;
    QWidget *centralwidget;
    QVBoxLayout *mainVBox;
    QSplitter *vSplitter;
    QSplitter *topRowSplitter;
    VideoFeedWidget *videoFeed;
    SignalChart *signalChart;
    WavepacketChart *wavepacketChart;
    QSplitter *botRowSplitter;
    LapTimeChart *lapTimeChart;
    QWidget *eventLogContainer;
    QVBoxLayout *eventLogLayout;
    QLabel *eventLogHeader;
    QPlainTextEdit *eventLogView;
    QWidget *rightPanelContainer;
    QHBoxLayout *rightPanelHBox;
    QTabWidget *dbTabs;
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
    QPushButton *btnHelpDesk;
    QPushButton *btnAccount;
    QPushButton *btnLiveFeed;
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
        MainWindow->setAutoFillBackground(false);
        MainWindow->setStyleSheet(QString::fromUtf8("background-color: #bcbcbc;"));
        actionSetIteration = new QAction(MainWindow);
        actionSetIteration->setObjectName("actionSetIteration");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/icons/cloud.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionSetIteration->setIcon(icon1);
        actionRunQuery = new QAction(MainWindow);
        actionRunQuery->setObjectName("actionRunQuery");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/icons/rocket.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionRunQuery->setIcon(icon2);
        actionRequestData = new QAction(MainWindow);
        actionRequestData->setObjectName("actionRequestData");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/icons/bubble.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionRequestData->setIcon(icon3);
        actionRefresh = new QAction(MainWindow);
        actionRefresh->setObjectName("actionRefresh");
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/icons/sync.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionRefresh->setIcon(icon4);
        actionDropDbTable = new QAction(MainWindow);
        actionDropDbTable->setObjectName("actionDropDbTable");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/icons/leaf.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionDropDbTable->setIcon(icon5);
        actionClearLog = new QAction(MainWindow);
        actionClearLog->setObjectName("actionClearLog");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/icons/trash.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionClearLog->setIcon(icon6);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/icons/exit.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionExit->setIcon(icon7);
        actionOpenVideo = new QAction(MainWindow);
        actionOpenVideo->setObjectName("actionOpenVideo");
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/icons/folder-open.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionOpenVideo->setIcon(icon8);
        actionPlayVideo = new QAction(MainWindow);
        actionPlayVideo->setObjectName("actionPlayVideo");
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/icons/play.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionPlayVideo->setIcon(icon9);
        actionPauseVideo = new QAction(MainWindow);
        actionPauseVideo->setObjectName("actionPauseVideo");
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icons/icons/pause.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionPauseVideo->setIcon(icon10);
        actionStopVideo = new QAction(MainWindow);
        actionStopVideo->setObjectName("actionStopVideo");
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/icons/stop.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionStopVideo->setIcon(icon11);
        actionLiveCamera = new QAction(MainWindow);
        actionLiveCamera->setObjectName("actionLiveCamera");
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/icons/icons/camera.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionLiveCamera->setIcon(icon12);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainVBox = new QVBoxLayout(centralwidget);
        mainVBox->setSpacing(4);
        mainVBox->setObjectName("mainVBox");
        mainVBox->setContentsMargins(4, 4, 4, 4);
        vSplitter = new QSplitter(centralwidget);
        vSplitter->setObjectName("vSplitter");
        vSplitter->setOrientation(Qt::Orientation::Vertical);
        vSplitter->setChildrenCollapsible(false);
        topRowSplitter = new QSplitter(vSplitter);
        topRowSplitter->setObjectName("topRowSplitter");
        topRowSplitter->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 0, 0);"));
        topRowSplitter->setOrientation(Qt::Orientation::Horizontal);
        topRowSplitter->setChildrenCollapsible(false);
        videoFeed = new VideoFeedWidget(topRowSplitter);
        videoFeed->setObjectName("videoFeed");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(50);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(videoFeed->sizePolicy().hasHeightForWidth());
        videoFeed->setSizePolicy(sizePolicy);
        videoFeed->setMinimumSize(QSize(640, 360));
        topRowSplitter->addWidget(videoFeed);
        signalChart = new SignalChart(topRowSplitter);
        signalChart->setObjectName("signalChart");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(25);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(signalChart->sizePolicy().hasHeightForWidth());
        signalChart->setSizePolicy(sizePolicy1);
        signalChart->setMinimumSize(QSize(250, 180));
        topRowSplitter->addWidget(signalChart);
        wavepacketChart = new WavepacketChart(topRowSplitter);
        wavepacketChart->setObjectName("wavepacketChart");
        sizePolicy1.setHeightForWidth(wavepacketChart->sizePolicy().hasHeightForWidth());
        wavepacketChart->setSizePolicy(sizePolicy1);
        wavepacketChart->setMinimumSize(QSize(250, 180));
        topRowSplitter->addWidget(wavepacketChart);
        vSplitter->addWidget(topRowSplitter);
        botRowSplitter = new QSplitter(vSplitter);
        botRowSplitter->setObjectName("botRowSplitter");
        botRowSplitter->setOrientation(Qt::Orientation::Horizontal);
        botRowSplitter->setChildrenCollapsible(false);
        lapTimeChart = new LapTimeChart(botRowSplitter);
        lapTimeChart->setObjectName("lapTimeChart");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(35);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lapTimeChart->sizePolicy().hasHeightForWidth());
        lapTimeChart->setSizePolicy(sizePolicy2);
        lapTimeChart->setMinimumSize(QSize(250, 180));
        lapTimeChart->setStyleSheet(QString::fromUtf8("background-color: rgb(203, 203, 203);"));
        botRowSplitter->addWidget(lapTimeChart);
        eventLogContainer = new QWidget(botRowSplitter);
        eventLogContainer->setObjectName("eventLogContainer");
        eventLogLayout = new QVBoxLayout(eventLogContainer);
        eventLogLayout->setSpacing(2);
        eventLogLayout->setObjectName("eventLogLayout");
        eventLogLayout->setContentsMargins(2, 2, 2, 2);
        eventLogHeader = new QLabel(eventLogContainer);
        eventLogHeader->setObjectName("eventLogHeader");
        QFont font;
        font.setPointSize(9);
        font.setBold(true);
        eventLogHeader->setFont(font);
        eventLogHeader->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));

        eventLogLayout->addWidget(eventLogHeader);

        eventLogView = new QPlainTextEdit(eventLogContainer);
        eventLogView->setObjectName("eventLogView");
        eventLogView->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);\n"
"color: rgb(0, 255, 0);\n"
"font-family: 'Consolas', 'Courier New', monospace;\n"
"font-size: 9pt;\n"
"border: 1px solid #333333;"));
        eventLogView->setReadOnly(true);

        eventLogLayout->addWidget(eventLogView);

        botRowSplitter->addWidget(eventLogContainer);
        rightPanelContainer = new QWidget(botRowSplitter);
        rightPanelContainer->setObjectName("rightPanelContainer");
        rightPanelHBox = new QHBoxLayout(rightPanelContainer);
        rightPanelHBox->setSpacing(6);
        rightPanelHBox->setObjectName("rightPanelHBox");
        rightPanelHBox->setContentsMargins(2, 2, 2, 2);
        dbTabs = new QTabWidget(rightPanelContainer);
        dbTabs->setObjectName("dbTabs");
        dbTabs->setStyleSheet(QString::fromUtf8("QTabWidget::pane { border: 1px solid #c0c6cf; background: #fbfbfb; }\n"
"QTabBar::tab { background: #e9ecef; border: 1px solid #c0c6cf; padding: 4px 10px; margin-right: 2px; font-size: 11px; color: #000000; }\n"
"QTabBar::tab:selected { background: #ffffff; border-bottom-color: #ffffff; font-weight: bold; color: #000000; }"));
        tabPostgreSql = new QWidget();
        tabPostgreSql->setObjectName("tabPostgreSql");
        tabPgLayout = new QVBoxLayout(tabPostgreSql);
        tabPgLayout->setSpacing(5);
        tabPgLayout->setObjectName("tabPgLayout");
        tabPgLayout->setContentsMargins(6, 6, 6, 6);
        tabPgTitle = new QLabel(tabPostgreSql);
        tabPgTitle->setObjectName("tabPgTitle");
        tabPgTitle->setStyleSheet(QString::fromUtf8("font-size: 11px; font-weight: bold; color: #000000;"));

        tabPgLayout->addWidget(tabPgTitle);

        rPg1 = new QHBoxLayout();
        rPg1->setObjectName("rPg1");
        iterationEditPg = new QLineEdit(tabPostgreSql);
        iterationEditPg->setObjectName("iterationEditPg");
        iterationEditPg->setStyleSheet(QString::fromUtf8("padding: 3px; font-size: 11px; color: #000000;"));

        rPg1->addWidget(iterationEditPg);

        btnReqDataPg = new QPushButton(tabPostgreSql);
        btnReqDataPg->setObjectName("btnReqDataPg");
        btnReqDataPg->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));
        btnReqDataPg->setIcon(icon3);

        rPg1->addWidget(btnReqDataPg);


        tabPgLayout->addLayout(rPg1);

        rPg2 = new QHBoxLayout();
        rPg2->setObjectName("rPg2");
        btnSetIterPg = new QPushButton(tabPostgreSql);
        btnSetIterPg->setObjectName("btnSetIterPg");
        btnSetIterPg->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));

        rPg2->addWidget(btnSetIterPg);

        btnSavePg = new QPushButton(tabPostgreSql);
        btnSavePg->setObjectName("btnSavePg");
        btnSavePg->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));

        rPg2->addWidget(btnSavePg);


        tabPgLayout->addLayout(rPg2);

        spacerPg = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        tabPgLayout->addItem(spacerPg);

        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/icons/icons/Postgresql.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        dbTabs->addTab(tabPostgreSql, icon13, QString());
        tabSqlite = new QWidget();
        tabSqlite->setObjectName("tabSqlite");
        tabSqliteLayout = new QVBoxLayout(tabSqlite);
        tabSqliteLayout->setSpacing(5);
        tabSqliteLayout->setObjectName("tabSqliteLayout");
        tabSqliteLayout->setContentsMargins(6, 6, 6, 6);
        tabTitle = new QLabel(tabSqlite);
        tabTitle->setObjectName("tabTitle");
        tabTitle->setStyleSheet(QString::fromUtf8("font-size: 11px; font-weight: bold; color: #000000;"));

        tabSqliteLayout->addWidget(tabTitle);

        r1 = new QHBoxLayout();
        r1->setObjectName("r1");
        iterationEdit = new QLineEdit(tabSqlite);
        iterationEdit->setObjectName("iterationEdit");
        iterationEdit->setStyleSheet(QString::fromUtf8("padding: 3px; font-size: 11px; color: #000000;"));

        r1->addWidget(iterationEdit);

        btnReqData = new QPushButton(tabSqlite);
        btnReqData->setObjectName("btnReqData");
        btnReqData->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));
        btnReqData->setIcon(icon3);

        r1->addWidget(btnReqData);


        tabSqliteLayout->addLayout(r1);

        r2 = new QHBoxLayout();
        r2->setObjectName("r2");
        btnSetIter = new QPushButton(tabSqlite);
        btnSetIter->setObjectName("btnSetIter");
        btnSetIter->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));

        r2->addWidget(btnSetIter);

        btnSave = new QPushButton(tabSqlite);
        btnSave->setObjectName("btnSave");
        btnSave->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));

        r2->addWidget(btnSave);


        tabSqliteLayout->addLayout(r2);

        r3 = new QHBoxLayout();
        r3->setObjectName("r3");
        btnRunQuery = new QPushButton(tabSqlite);
        btnRunQuery->setObjectName("btnRunQuery");
        btnRunQuery->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));
        btnRunQuery->setIcon(icon2);

        r3->addWidget(btnRunQuery);

        btnPlotData = new QPushButton(tabSqlite);
        btnPlotData->setObjectName("btnPlotData");
        btnPlotData->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/icons/icons/chart-bars.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btnPlotData->setIcon(icon14);

        r3->addWidget(btnPlotData);


        tabSqliteLayout->addLayout(r3);

        altitudeLabel = new QLabel(tabSqlite);
        altitudeLabel->setObjectName("altitudeLabel");
        altitudeLabel->setStyleSheet(QString::fromUtf8("font-size: 11px; color: #000000;"));

        tabSqliteLayout->addWidget(altitudeLabel);

        positionLabel = new QLabel(tabSqlite);
        positionLabel->setObjectName("positionLabel");
        positionLabel->setStyleSheet(QString::fromUtf8("font-size: 11px; color: #000000;"));

        tabSqliteLayout->addWidget(positionLabel);

        rProg = new QHBoxLayout();
        rProg->setObjectName("rProg");
        lblProc = new QLabel(tabSqlite);
        lblProc->setObjectName("lblProc");
        lblProc->setStyleSheet(QString::fromUtf8("font-size: 11px;\n"
"color: rgb(0, 0, 0);"));

        rProg->addWidget(lblProc);

        progressBar = new QProgressBar(tabSqlite);
        progressBar->setObjectName("progressBar");
        progressBar->setStyleSheet(QString::fromUtf8("QProgressBar { border: 1px solid #c0c6cf; border-radius: 2px; text-align: center; height: 16px; font-size: 10px; color: #000000; }\n"
"QProgressBar::chunk { background-color: #3b82f6; }"));
        progressBar->setValue(0);
        progressBar->setTextVisible(true);

        rProg->addWidget(progressBar);


        tabSqliteLayout->addLayout(rProg);

        warnLbl = new QLabel(tabSqlite);
        warnLbl->setObjectName("warnLbl");
        warnLbl->setStyleSheet(QString::fromUtf8("color: #000000; font-weight: bold; font-size: 10px;"));
        warnLbl->setAlignment(Qt::AlignmentFlag::AlignCenter);

        tabSqliteLayout->addWidget(warnLbl);

        rWarn1 = new QHBoxLayout();
        rWarn1->setObjectName("rWarn1");
        btnDrop1 = new QPushButton(tabSqlite);
        btnDrop1->setObjectName("btnDrop1");
        btnDrop1->setStyleSheet(QString::fromUtf8("padding: 3px; font-size: 10px; color: #000000;"));

        rWarn1->addWidget(btnDrop1);

        btnDisconn1 = new QPushButton(tabSqlite);
        btnDisconn1->setObjectName("btnDisconn1");
        btnDisconn1->setStyleSheet(QString::fromUtf8("padding: 3px; font-size: 10px; color: #000000;"));

        rWarn1->addWidget(btnDisconn1);


        tabSqliteLayout->addLayout(rWarn1);

        rWarn2 = new QHBoxLayout();
        rWarn2->setObjectName("rWarn2");
        btnDisconn2 = new QPushButton(tabSqlite);
        btnDisconn2->setObjectName("btnDisconn2");
        btnDisconn2->setStyleSheet(QString::fromUtf8("padding: 3px; font-size: 10px; color: #000000;"));

        rWarn2->addWidget(btnDisconn2);

        btnDrop2 = new QPushButton(tabSqlite);
        btnDrop2->setObjectName("btnDrop2");
        btnDrop2->setStyleSheet(QString::fromUtf8("padding: 3px; font-size: 10px; color: #000000;"));
        btnDrop2->setIcon(icon5);

        rWarn2->addWidget(btnDrop2);


        tabSqliteLayout->addLayout(rWarn2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        tabSqliteLayout->addItem(verticalSpacer);

        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/icons/icons/sqlite_icon.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        dbTabs->addTab(tabSqlite, icon15, QString());
        tabMySql = new QWidget();
        tabMySql->setObjectName("tabMySql");
        tabMySqlLayout = new QVBoxLayout(tabMySql);
        tabMySqlLayout->setSpacing(5);
        tabMySqlLayout->setObjectName("tabMySqlLayout");
        tabMySqlLayout->setContentsMargins(6, 6, 6, 6);
        tabMySqlTitle = new QLabel(tabMySql);
        tabMySqlTitle->setObjectName("tabMySqlTitle");
        tabMySqlTitle->setStyleSheet(QString::fromUtf8("font-size: 11px; font-weight: bold; color: #000000;"));

        tabMySqlLayout->addWidget(tabMySqlTitle);

        rMySql1 = new QHBoxLayout();
        rMySql1->setObjectName("rMySql1");
        iterationEditMySql = new QLineEdit(tabMySql);
        iterationEditMySql->setObjectName("iterationEditMySql");
        iterationEditMySql->setStyleSheet(QString::fromUtf8("padding: 3px; font-size: 11px; color: #000000;"));

        rMySql1->addWidget(iterationEditMySql);

        btnReqDataMySql = new QPushButton(tabMySql);
        btnReqDataMySql->setObjectName("btnReqDataMySql");
        btnReqDataMySql->setStyleSheet(QString::fromUtf8("padding: 3px 6px; font-size: 11px; color: #000000;"));
        btnReqDataMySql->setIcon(icon3);

        rMySql1->addWidget(btnReqDataMySql);


        tabMySqlLayout->addLayout(rMySql1);

        spacerMySql = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        tabMySqlLayout->addItem(spacerMySql);

        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/icons/icons/Mysql_app.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        dbTabs->addTab(tabMySql, icon16, QString());

        rightPanelHBox->addWidget(dbTabs);

        connGroup = new QGroupBox(rightPanelContainer);
        connGroup->setObjectName("connGroup");
        connGroup->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
        connLayout = new QVBoxLayout(connGroup);
        connLayout->setSpacing(4);
        connLayout->setObjectName("connLayout");
        connLayout->setContentsMargins(6, 6, 6, 6);
        lblIntIp = new QLabel(connGroup);
        lblIntIp->setObjectName("lblIntIp");
        lblIntIp->setStyleSheet(QString::fromUtf8("font-size: 10px; font-weight: normal; color: #000000;"));

        connLayout->addWidget(lblIntIp);

        internalIpEdit = new QLineEdit(connGroup);
        internalIpEdit->setObjectName("internalIpEdit");
        internalIpEdit->setStyleSheet(QString::fromUtf8("padding: 2px 4px; font-size: 10px; color: #000000;"));
        internalIpEdit->setReadOnly(true);

        connLayout->addWidget(internalIpEdit);

        lblExtIp = new QLabel(connGroup);
        lblExtIp->setObjectName("lblExtIp");
        lblExtIp->setStyleSheet(QString::fromUtf8("font-size: 10px; font-weight: normal; color: #000000;"));

        connLayout->addWidget(lblExtIp);

        externalIpEdit = new QLineEdit(connGroup);
        externalIpEdit->setObjectName("externalIpEdit");
        externalIpEdit->setStyleSheet(QString::fromUtf8("padding: 2px 4px; font-size: 10px; color: #000000;"));
        externalIpEdit->setReadOnly(true);

        connLayout->addWidget(externalIpEdit);

        lblStatusTitle = new QLabel(connGroup);
        lblStatusTitle->setObjectName("lblStatusTitle");
        lblStatusTitle->setStyleSheet(QString::fromUtf8("font-size: 10px; font-weight: normal; color: #000000;"));

        connLayout->addWidget(lblStatusTitle);

        statusLabel = new QLabel(connGroup);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setStyleSheet(QString::fromUtf8("background: #e8f5e9; color: #000000; border: 1px solid #a5d6a7; border-radius: 2px; padding: 3px; font-weight: bold; font-size: 10px;"));
        statusLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        connLayout->addWidget(statusLabel);

        btnHelpDesk = new QPushButton(connGroup);
        btnHelpDesk->setObjectName("btnHelpDesk");
        btnHelpDesk->setStyleSheet(QString::fromUtf8("padding: 4px; font-size: 10px; color: #000000;"));

        connLayout->addWidget(btnHelpDesk);

        btnAccount = new QPushButton(connGroup);
        btnAccount->setObjectName("btnAccount");
        btnAccount->setStyleSheet(QString::fromUtf8("padding: 4px; font-size: 10px; color: #000000;"));

        connLayout->addWidget(btnAccount);

        btnLiveFeed = new QPushButton(connGroup);
        btnLiveFeed->setObjectName("btnLiveFeed");
        btnLiveFeed->setStyleSheet(QString::fromUtf8("padding: 4px; font-size: 10px;\n"
"color: rgb(0, 0, 0);"));

        connLayout->addWidget(btnLiveFeed);

        spacerConn = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        connLayout->addItem(spacerConn);

        btnExit = new QPushButton(connGroup);
        btnExit->setObjectName("btnExit");
        btnExit->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
        btnExit->setIcon(icon7);

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
        mainToolBar->setStyleSheet(QString::fromUtf8("QToolBar { background: #f0f2f5; border-bottom: 1px solid #d0d4dc; spacing: 4px; padding: 2px 6px; color: #000000; }\n"
"QToolButton { background: transparent; border: 1px solid transparent; border-radius: 3px; padding: 4px 8px; font-size: 11px; color: #000000; }\n"
"QToolButton:hover { background: #e2e6ea; border-color: #c0c6cf; color: #000000; }\n"
"QToolButton:pressed { background: #d0d6de; color: #000000; }"));
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
        mainToolBar->addAction(actionExit);

        retranslateUi(MainWindow);

        dbTabs->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "CoffeeBreak V1.0", nullptr));
        actionSetIteration->setText(QCoreApplication::translate("MainWindow", "Set Iteration", nullptr));
        actionRunQuery->setText(QCoreApplication::translate("MainWindow", "Run Query", nullptr));
        actionRequestData->setText(QCoreApplication::translate("MainWindow", "Request Data", nullptr));
        actionRefresh->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
        actionDropDbTable->setText(QCoreApplication::translate("MainWindow", "Drop DB Table", nullptr));
        actionClearLog->setText(QCoreApplication::translate("MainWindow", "Clear Log", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
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
        eventLogHeader->setText(QCoreApplication::translate("MainWindow", "Event log", nullptr));
        tabPgTitle->setText(QCoreApplication::translate("MainWindow", "PostgreSQL database managment", nullptr));
        iterationEditPg->setText(QCoreApplication::translate("MainWindow", "8", nullptr));
        btnReqDataPg->setText(QCoreApplication::translate("MainWindow", "Request data", nullptr));
        btnSetIterPg->setText(QCoreApplication::translate("MainWindow", "Set Iteration", nullptr));
        btnSavePg->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        dbTabs->setTabText(dbTabs->indexOf(tabPostgreSql), QCoreApplication::translate("MainWindow", "PostgreSQL", nullptr));
        tabTitle->setText(QCoreApplication::translate("MainWindow", "SQLite database managment", nullptr));
        iterationEdit->setText(QCoreApplication::translate("MainWindow", "8", nullptr));
        btnReqData->setText(QCoreApplication::translate("MainWindow", "Request data", nullptr));
        btnSetIter->setText(QCoreApplication::translate("MainWindow", "Set Iteration", nullptr));
        btnSave->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        btnRunQuery->setText(QCoreApplication::translate("MainWindow", "Run Query", nullptr));
        btnPlotData->setText(QCoreApplication::translate("MainWindow", "Plot Data", nullptr));
        altitudeLabel->setText(QCoreApplication::translate("MainWindow", "Altitude: 124 m", nullptr));
        positionLabel->setText(QCoreApplication::translate("MainWindow", "Current Position: Sector A - Lobby", nullptr));
        lblProc->setText(QCoreApplication::translate("MainWindow", "Process(%):", nullptr));
        warnLbl->setText(QCoreApplication::translate("MainWindow", "WARNING! All data will pemanently deleted!", nullptr));
        btnDrop1->setText(QCoreApplication::translate("MainWindow", "Drop DB Table", nullptr));
        btnDisconn1->setText(QCoreApplication::translate("MainWindow", "Disconnect Database", nullptr));
        btnDisconn2->setText(QCoreApplication::translate("MainWindow", "Disconnect", nullptr));
        btnDrop2->setText(QCoreApplication::translate("MainWindow", "Drop DB Table", nullptr));
        dbTabs->setTabText(dbTabs->indexOf(tabSqlite), QCoreApplication::translate("MainWindow", "SQLite", nullptr));
        tabMySqlTitle->setText(QCoreApplication::translate("MainWindow", "MySQL database managment", nullptr));
        iterationEditMySql->setText(QCoreApplication::translate("MainWindow", "8", nullptr));
        btnReqDataMySql->setText(QCoreApplication::translate("MainWindow", "Request data", nullptr));
        dbTabs->setTabText(dbTabs->indexOf(tabMySql), QCoreApplication::translate("MainWindow", "MySQL", nullptr));
        connGroup->setTitle(QCoreApplication::translate("MainWindow", "Connection details", nullptr));
        lblIntIp->setText(QCoreApplication::translate("MainWindow", "Client Internal IP", nullptr));
        internalIpEdit->setText(QCoreApplication::translate("MainWindow", "127.0.0.1", nullptr));
        lblExtIp->setText(QCoreApplication::translate("MainWindow", "Client External IP", nullptr));
        externalIpEdit->setText(QCoreApplication::translate("MainWindow", "192.168.10.192", nullptr));
        lblStatusTitle->setText(QCoreApplication::translate("MainWindow", "Status", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "Online", nullptr));
        btnHelpDesk->setText(QCoreApplication::translate("MainWindow", "Online HelpDesk", nullptr));
        btnAccount->setText(QCoreApplication::translate("MainWindow", "My Account", nullptr));
        btnLiveFeed->setText(QCoreApplication::translate("MainWindow", "Live Feed", nullptr));
        btnExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        mainToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "MainToolbar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
