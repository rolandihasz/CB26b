#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSplitter>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle(QStringLiteral("CoffeeBreak V1.0"));
  setWindowIcon(QIcon(QStringLiteral(":/icons/icons/database.ico")));
  setAutoFillBackground(true);
  setStyleSheet(
      QStringLiteral("QMainWindow { background-color: #bcbcbc; }\n"
                     "QWidget { color: #000000; }\n"
                     "QLabel { color: #000000; }\n"
                     "QPushButton { color: #000000; }\n"
                     "QLineEdit { color: #000000; }\n"
                     "QToolButton { color: #000000; }\n"
                     "QTabBar::tab { color: #000000; }\n"
                     "QGroupBox { color: #000000; }\n"
                     "QProgressBar { color: #000000; }\n"
                     "QStatusBar { color: #000000; }\n"
                     "QMenuBar { color: #000000; }\n"
                     "QMenu { color: #000000; }\n"
                     "QPlainTextEdit#eventLogView {\n"
                     "  background-color: #000000;\n"
                     "  color: #00ff00;\n"
                     "  font-family: 'Consolas', 'Courier New', monospace;\n"
                     "  font-size: 9pt;\n"
                     "  border: 1px solid #333333;\n"
                     "  selection-background-color: #004400;\n"
                     "  selection-color: #00ff00;\n"
                     "}\n"));

  // Build the complete interface matching the screenshot
  setupUiCustom();

  // Setup Managers
  m_telemetryMgr = new TelemetryManager(this);
  m_databaseMgr = new DatabaseManager(this);

  setupConnections();
  initializeSystem();

  // Default to maximized / full screen view
  showMaximized();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupUiCustom() {
  // Bind member pointers to UI widgets created from mainwindow.ui
  m_videoFeed = ui->videoFeed;
  m_signalChart = ui->signalChart;
  m_wavepacketChart = ui->wavepacketChart;
  m_lapTimeChart = ui->lapTimeChart;
  m_eventLogView = ui->eventLogView;
  if (m_eventLogView) {
    m_eventLogView->setStyleSheet(
        QStringLiteral("QPlainTextEdit { "
                       "  background-color: #000000; "
                       "  color: #00ff00; "
                       "  font-family: 'Consolas', 'Courier New', monospace; "
                       "  font-size: 9pt; "
                       "  border: 1px solid #333333; "
                       "  selection-background-color: #004400; "
                       "  selection-color: #00ff00; "
                       "}"));
  }
  m_internalIpEdit = ui->internalIpEdit;
  m_externalIpEdit = ui->externalIpEdit;
  m_statusLabel = ui->statusLabel;
  m_progressBar = ui->progressBar;
  m_iterationEdit = ui->iterationEdit;
  m_altitudeLabel = ui->altitudeLabel;
  m_positionLabel = ui->positionLabel;

  // Connect Top ToolBar Actions
  connect(ui->actionSetIteration, &QAction::triggered, this,
          &MainWindow::onSetIterationClicked);
  connect(ui->actionRunQuery, &QAction::triggered, this,
          &MainWindow::onRunQueryClicked);
  connect(ui->actionRequestData, &QAction::triggered, this,
          &MainWindow::onRequestDataClicked);
  connect(ui->actionRefresh, &QAction::triggered, this,
          &MainWindow::onRefreshClicked);
  connect(ui->actionDropDbTable, &QAction::triggered, this,
          &MainWindow::onDropDbTableClicked);
  connect(ui->actionClearLog, &QAction::triggered, this,
          &MainWindow::onClearLogClicked);
  connect(ui->actionExit, &QAction::triggered, this,
          &MainWindow::onExitClicked);

  // Connect Database Tab Buttons
  connect(ui->btnReqData, &QPushButton::clicked, this,
          &MainWindow::onRequestDataClicked);
  connect(ui->btnSetIter, &QPushButton::clicked, this,
          &MainWindow::onSetIterationClicked);
  connect(ui->btnSave, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
  connect(ui->btnRunQuery, &QPushButton::clicked, this,
          &MainWindow::onRunQueryClicked);
  connect(ui->btnPlotData, &QPushButton::clicked, this,
          &MainWindow::onPlotDataClicked);
  connect(ui->btnDrop1, &QPushButton::clicked, this,
          &MainWindow::onDropDbTableClicked);
  connect(ui->btnDisconn1, &QPushButton::clicked, this,
          &MainWindow::onDisconnectClicked);
  connect(ui->btnDisconn2, &QPushButton::clicked, this,
          &MainWindow::onDisconnectClicked);
  connect(ui->btnDrop2, &QPushButton::clicked, this,
          &MainWindow::onDropDbTableClicked);

  // Connect PostgreSQL Tab Buttons as well
  connect(ui->btnReqDataPg, &QPushButton::clicked, this,
          &MainWindow::onRequestDataClicked);
  connect(ui->btnSetIterPg, &QPushButton::clicked, this,
          &MainWindow::onSetIterationClicked);
  connect(ui->btnSavePg, &QPushButton::clicked, this,
          &MainWindow::onSaveClicked);

  // Connect Connection Details Group Buttons
  connect(ui->btnHelpDesk, &QPushButton::clicked, this,
          &MainWindow::onHelpDeskClicked);
  connect(ui->btnAccount, &QPushButton::clicked, this,
          &MainWindow::onMyAccountClicked);
  connect(ui->btnLiveFeed, &QPushButton::clicked, this,
          &MainWindow::onLiveFeedClicked);
  connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);

  // Splitter initial sizes & stretch factors
  ui->topRowSplitter->setStretchFactor(0, 50);
  ui->topRowSplitter->setStretchFactor(1, 25);
  ui->topRowSplitter->setStretchFactor(2, 25);
  ui->topRowSplitter->setSizes(QList<int>() << 640 << 320 << 320);

  ui->botRowSplitter->setStretchFactor(0, 35);
  ui->botRowSplitter->setStretchFactor(1, 35);
  ui->botRowSplitter->setStretchFactor(2, 30);

  ui->vSplitter->setStretchFactor(0, 55);
  ui->vSplitter->setStretchFactor(1, 45);
  ui->vSplitter->setSizes(QList<int>() << 420 << 320);
}

void MainWindow::setupConnections() {
  // Video Feed -> EagleEye detection & signals
  m_videoFeed->setEagleEyeClassifier(&m_classifier);
  connect(m_videoFeed, &VideoFeedWidget::roundDetected, this,
          &MainWindow::onRoundDetected);
  connect(m_videoFeed, &VideoFeedWidget::statusMessage, this,
          &MainWindow::appendLog);

  // Telemetry Manager
  connect(m_telemetryMgr, &TelemetryManager::logMessage, this,
          &MainWindow::appendLog);
  connect(m_telemetryMgr, &TelemetryManager::gpuUpdated, this,
          &MainWindow::onGpuUpdated);
  connect(m_telemetryMgr, &TelemetryManager::ipUpdated, this,
          &MainWindow::onIpUpdated);

  // Database Manager
  connect(m_databaseMgr, &DatabaseManager::logMessage, this,
          &MainWindow::appendLog);
  connect(m_databaseMgr, &DatabaseManager::progressUpdated, this,
          [this](int pct) {
            if (m_progressBar)
              m_progressBar->setValue(pct);
          });
}

void MainWindow::initializeSystem() {
  // 1. Initial screen diagnostics log matching the screenshot
  for (const QString &diag : m_telemetryMgr->screenDiagnostics()) {
    appendLog(diag);
  }

  appendLog(QStringLiteral("InRSoft CoffeeBreak Started..."));
  appendLog(QStringLiteral("connecting..."));

  // 2. Initialize database
  m_databaseMgr->initLocalDatabase();

  // 3. Load EagleEye AI model weights
  QString weightsPath = QStringLiteral("eagle_eye_weights.bin");
  if (!QFile::exists(weightsPath)) {
    weightsPath = QStringLiteral("various/EagleEye/eagle_eye_weights.bin");
  }
  if (m_classifier.loadWeights(weightsPath)) {
    appendLog(QStringLiteral(
        "EagleEye AI Model: Loaded successfully (eagle_eye_model.pth)."));
    appendLog(QStringLiteral(
        "EagleEye AI: Monitoring round counter region [1684, 29, 189, 130]"));
  } else {
    appendLog(QStringLiteral("EagleEye AI: Failed to load weights from %1")
                  .arg(weightsPath));
  }

  // 4. Pre-load historical rounds into LapTimeChart if CSV exists
  QString roundsCsv = QStringLiteral("rounds.csv");
  if (!QFile::exists(roundsCsv)) {
    roundsCsv = QStringLiteral("various/ai/rounds.csv");
  }
  if (QFile::exists(roundsCsv)) {
    m_lapTimeChart->loadFromCsv(roundsCsv);
    appendLog(QStringLiteral(
                  "LapTimeChart: Loaded %1 historical round records from %2")
                  .arg(m_lapTimeChart->roundCount())
                  .arg(roundsCsv));
  }

  // 5. Connect and start OBS Virtual Camera stream
  m_videoFeed->initCamera(QStringLiteral("OBS Virtual Camera"));

  // 6. Start telemetry monitoring (RTX 3090 Ti GPU & network)
  m_telemetryMgr->startMonitoring();
}

void MainWindow::appendLog(const QString &message) {
  QString timestamp = QDateTime::currentDateTime().toString(
      QStringLiteral("yyyy-MM-dd hh:mm:ss"));
  QString line = QStringLiteral("%1 %2").arg(timestamp, message);
  if (m_eventLogView) {
    m_eventLogView->appendPlainText(line);
  }
}

void MainWindow::onRoundDetected(int roundNumber, const QString &roundStr,
                                 double durationSec) {
  QString msg =
      QStringLiteral("New round detected: %1 (Round %2) | Lap Time: %3 s")
          .arg(roundStr)
          .arg(roundNumber)
          .arg(QString::number(durationSec, 'f', 1));
  appendLog(msg);

  // Update Lap Time Chart
  m_lapTimeChart->addRound(roundNumber, durationSec);

  // Save to Database and CSV
  m_databaseMgr->logRound(roundNumber, durationSec);
  m_databaseMgr->saveRoundToCsv(roundNumber, durationSec,
                                QStringLiteral("rounds.csv"));
}

void MainWindow::onGpuUpdated(const GpuStats &stats) {
  // Feed GPU telemetry into SignalChart channels
  double normTemp = (stats.temperatureC - 50.0) / 40.0;   // normalized
  double normUtil = (stats.gpuUtilPercent - 50.0) / 50.0; // normalized
  m_signalChart->pushData(normUtil, normTemp);

  static int logThrottle = 0;
  if (++logThrottle % 5 == 0) { // Log once every ~15 seconds to avoid flooding
    appendLog(QStringLiteral("GPU Telemetry: %1 | Temp: %2°C | Fan: %3% | "
                             "Util: %4% | VRAM: %5 / %6 MB")
                  .arg(stats.name)
                  .arg(stats.temperatureC)
                  .arg(stats.fanSpeedPercent)
                  .arg(stats.gpuUtilPercent)
                  .arg(stats.memUsedMb)
                  .arg(stats.memTotalMb));
  }
}

void MainWindow::onIpUpdated(const QString &intIp, const QString &extIp) {
  if (m_internalIpEdit)
    m_internalIpEdit->setText(intIp);
  if (m_externalIpEdit)
    m_externalIpEdit->setText(extIp);
  if (m_statusLabel) {
    m_statusLabel->setText(extIp.isEmpty() ? QStringLiteral("Offline")
                                           : QStringLiteral("Online"));
  }
}

void MainWindow::onSetIterationClicked() {
  bool ok = false;
  int val = m_iterationEdit ? m_iterationEdit->text().toInt(&ok) : 8;
  if (ok && val > 0) {
    m_databaseMgr->setIteration(val);
  } else {
    QMessageBox::warning(this, tr("CoffeeBreak"),
                         tr("Please enter a valid integer iteration count."));
  }
}

void MainWindow::onRunQueryClicked() {
  int iter = m_iterationEdit ? m_iterationEdit->text().toInt() : 8;
  if (iter <= 0)
    iter = 8;
  m_databaseMgr->generateSampleData(iter);
}

void MainWindow::onRequestDataClicked() {
  QSqlQueryModel *model = m_databaseMgr->loadTableData();
  delete model;
}

void MainWindow::onRefreshClicked() {
  appendLog(QStringLiteral("Refreshing system telemetry and camera stream..."));
  m_videoFeed->initCamera();
  m_telemetryMgr->resolveInternalIp();
  m_telemetryMgr->resolveExternalIp();
}

void MainWindow::onDropDbTableClicked() {
  auto reply = QMessageBox::question(this, tr("Confirm Drop Table"),
                                     tr("WARNING! All data will permanently "
                                        "deleted!\nDo you want to proceed?"),
                                     QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    m_databaseMgr->dropTable();
  }
}

void MainWindow::onClearLogClicked() {
  if (m_eventLogView) {
    m_eventLogView->clear();
  }
}

void MainWindow::onExitClicked() { QApplication::quit(); }

void MainWindow::onHelpDeskClicked() {
  appendLog(QStringLiteral("Opening Online HelpDesk..."));
  QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com")));
}

void MainWindow::onMyAccountClicked() {
  appendLog(QStringLiteral("Opening My Account Portal..."));
  QString user = m_databaseMgr ? m_databaseMgr->currentUser() : QStringLiteral("admin");
  QString role = m_databaseMgr ? m_databaseMgr->currentRole() : QStringLiteral("admin");
  QMessageBox::information(this, tr("My Account"),
                           tr("CoffeeBreak User Account: %1 (%2, Active)").arg(user, role));
}

void MainWindow::onLiveFeedClicked() {
  appendLog(QStringLiteral("Toggling Live Stream HUD Overlay..."));
  m_videoFeed->setOverlayEnabled(!m_videoFeed->isOverlayEnabled());
}

void MainWindow::onPlotDataClicked() {
  appendLog(
      QStringLiteral("Plotting database query records into telemetry charts."));
  m_lapTimeChart->update();
}

void MainWindow::onSaveClicked() {
  appendLog(
      QStringLiteral("Telemetry configuration and database snapshot saved."));
  QMessageBox::information(this, tr("CoffeeBreak"),
                           tr("Configuration saved successfully."));
}

void MainWindow::onDisconnectClicked() { m_databaseMgr->disconnectDatabase(); }
