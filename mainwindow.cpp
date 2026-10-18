#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QSplitter>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle(QStringLiteral("CoffeeBreak V1.0 Telemetry Workstation"));
  setWindowIcon(QIcon(QStringLiteral(":/icons/icons/database.ico")));
  setAutoFillBackground(true);

  // Build the complete interface matching the design
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
  m_internalIpEdit = ui->internalIpEdit;
  m_externalIpEdit = ui->externalIpEdit;
  m_statusLabel = ui->statusLabel;
  m_progressBar = ui->progressBar;
  m_iterationEdit = ui->iterationEdit;
  m_altitudeLabel = ui->altitudeLabel;
  m_positionLabel = ui->positionLabel;

  // Connect Top ToolBar Actions for Video Playback & Analysis
  connect(ui->actionOpenVideo, &QAction::triggered, this,
          &MainWindow::onOpenVideoClicked);
  connect(ui->actionPlayVideo, &QAction::triggered, this,
          &MainWindow::onPlayVideoClicked);
  connect(ui->actionPauseVideo, &QAction::triggered, this,
          &MainWindow::onPauseVideoClicked);
  connect(ui->actionStopVideo, &QAction::triggered, this,
          &MainWindow::onStopVideoClicked);
  connect(ui->actionLiveCamera, &QAction::triggered, this,
          &MainWindow::onLiveCameraClicked);
  connect(ui->actionClearLog, &QAction::triggered, this,
          &MainWindow::onClearLogClicked);
  connect(ui->actionExit, &QAction::triggered, this,
          &MainWindow::onExitClicked);

  // Quick Load Test Gameplay Button
  m_quickTestBtn = new QPushButton(QStringLiteral("Load Test Gameplay"), this);
  m_quickTestBtn->setToolTip(QStringLiteral("Quick load D:\\Containers\\testdata\\pre-recorded-gameplay.mp4"));
  m_quickTestBtn->setStyleSheet(
      QStringLiteral("QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0284c7, stop:1 #0369a1); border: 1px solid #38bdf8; color: #ffffff; font-weight: bold; padding: 4px 10px; border-radius: 4px; font-size: 11px; margin-right: 4px; }"
                     "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0ea5e9, stop:1 #0284c7); border-color: #7dd3fc; color: #ffffff; }"
                     "QPushButton:pressed { background: #0c4a6e; }"));
  connect(m_quickTestBtn, &QPushButton::clicked, this, &MainWindow::onLoadTestGameplayClicked);
  ui->mainToolBar->insertWidget(ui->actionClearLog, m_quickTestBtn);

  // Speed Selector ComboBox
  QLabel *speedLbl = new QLabel(QStringLiteral(" Speed: "), this);
  speedLbl->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 600; color: #94a3b8;"));
  ui->mainToolBar->insertWidget(ui->actionClearLog, speedLbl);

  m_speedCombo = new QComboBox(this);
  m_speedCombo->addItems({QStringLiteral("1.0x (Normal)"), QStringLiteral("1.5x"), QStringLiteral("2.0x (Fast)"), QStringLiteral("4.0x (Turbo)")});
  m_speedCombo->setStyleSheet(
      QStringLiteral("QComboBox { background-color: #0b0f19; border: 1px solid #334155; border-radius: 4px; padding: 3px 8px; font-size: 11px; color: #f8fafc; min-width: 105px; }"
                     "QComboBox:hover { border-color: #38bdf8; }"
                     "QComboBox QAbstractItemView { background-color: #0f172a; color: #f8fafc; selection-background-color: #1e293b; selection-color: #38bdf8; border: 1px solid #334155; }"));
  connect(m_speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSpeedChanged);
  ui->mainToolBar->insertWidget(ui->actionClearLog, m_speedCombo);

  // Video Time Slider (Seeker)
  QLabel *timeLbl = new QLabel(QStringLiteral(" Position: "), this);
  timeLbl->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 600; color: #94a3b8; margin-left: 6px;"));
  ui->mainToolBar->insertWidget(ui->actionClearLog, timeLbl);

  m_timeSlider = new QSlider(Qt::Horizontal, this);
  m_timeSlider->setRange(0, 100);
  m_timeSlider->setValue(0);
  m_timeSlider->setMinimumWidth(160);
  m_timeSlider->setMaximumWidth(280);
  m_timeSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  m_timeSlider->setStyleSheet(
      QStringLiteral("QSlider::groove:horizontal { height: 6px; background: #1e293b; border-radius: 3px; }"
                     "QSlider::sub-page:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0284c7, stop:1 #38bdf8); border-radius: 3px; }"
                     "QSlider::handle:horizontal { background: #38bdf8; width: 14px; margin-top: -4px; margin-bottom: -4px; border-radius: 7px; border: 1px solid #ffffff; }"
                     "QSlider::handle:horizontal:hover { background: #7dd3fc; }"));
  connect(m_timeSlider, &QSlider::sliderPressed, this, [this]() { m_userIsSeeking = true; });
  connect(m_timeSlider, &QSlider::sliderReleased, this, [this]() {
    m_userIsSeeking = false;
    onSliderSeek(m_timeSlider->value());
  });
  connect(m_timeSlider, &QSlider::sliderMoved, this, &MainWindow::onSliderSeek);
  ui->mainToolBar->insertWidget(ui->actionClearLog, m_timeSlider);

  m_timeLabel = new QLabel(QStringLiteral("00:00 / 00:00"), this);
  m_timeLabel->setStyleSheet(QStringLiteral("font-family: 'Consolas', monospace; font-size: 11px; color: #38bdf8; padding-left: 4px; padding-right: 10px; font-weight: bold;"));
  ui->mainToolBar->insertWidget(ui->actionClearLog, m_timeLabel);

  ui->mainToolBar->insertSeparator(ui->actionClearLog);

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
  m_btnLiveFeed = ui->btnLiveFeed;
  if (m_btnLiveFeed) {
    connect(m_btnLiveFeed, &QPushButton::clicked, this,
            &MainWindow::onLiveFeedClicked);
  }
  m_btnVideoToggle = findChild<QPushButton*>("btnVideoToggle");
  if (m_btnVideoToggle) {
    connect(m_btnVideoToggle, &QPushButton::clicked, this,
            &MainWindow::onLiveFeedClicked);
  }
  connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);
  updateLiveFeedButtonState(true);

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
  connect(m_videoFeed, &VideoFeedWidget::lapProgressChanged, this,
          &MainWindow::onLapProgressChanged);
  connect(m_videoFeed, &VideoFeedWidget::statusMessage, this,
          &MainWindow::appendLog);
  connect(m_videoFeed, &VideoFeedWidget::videoPositionChanged, this,
          &MainWindow::onVideoPositionChanged);
  connect(m_videoFeed, &VideoFeedWidget::videoDurationChanged, this,
          &MainWindow::onVideoDurationChanged);
  connect(m_videoFeed, &VideoFeedWidget::playbackStateChanged, this,
          &MainWindow::onPlaybackStateChanged);
  connect(m_videoFeed, &VideoFeedWidget::feedModeChanged, this,
          &MainWindow::onFeedModeChanged);

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
  QString weightsPath;
  const QStringList candidateWeights = {
      QCoreApplication::applicationDirPath() + QStringLiteral("/eagle_eye_weights.bin"),
      QCoreApplication::applicationDirPath() + QStringLiteral("/../eagle_eye_weights.bin"),
      QCoreApplication::applicationDirPath() + QStringLiteral("/../../eagle_eye_weights.bin"),
      QCoreApplication::applicationDirPath() + QStringLiteral("/../../../eagle_eye_weights.bin"),
      QStringLiteral("eagle_eye_weights.bin"),
      QStringLiteral("various/EagleEye/eagle_eye_weights.bin"),
      QStringLiteral("d:/cb26/cb26/eagle_eye_weights.bin"),
      QStringLiteral("d:/cb26/cb26/various/EagleEye/eagle_eye_weights.bin"),
      QStringLiteral("d:/Containers/CB26b/eagle_eye_weights.bin")
  };
  for (const QString &p : candidateWeights) {
    if (QFile::exists(p)) {
      weightsPath = p;
      break;
    }
  }

  if (!weightsPath.isEmpty() && m_classifier.loadWeights(weightsPath)) {
    appendLog(QStringLiteral(
        "EagleEye AI Model: Loaded successfully (%1).").arg(weightsPath));
    appendLog(QStringLiteral(
        "EagleEye AI: Monitoring round counter region [1684, 29, 189, 130]"));
  } else {
    appendLog(QStringLiteral("EagleEye AI: Failed to load weights from %1")
                  .arg(weightsPath.isEmpty() ? QStringLiteral("any candidate path") : weightsPath));
  }

  // 4. Pre-load historical rounds into LapTimeChart if CSV exists
  QString roundsCsv;
  const QStringList candidateCsvs = {
      QCoreApplication::applicationDirPath() + QStringLiteral("/rounds.csv"),
      QCoreApplication::applicationDirPath() + QStringLiteral("/../rounds.csv"),
      QCoreApplication::applicationDirPath() + QStringLiteral("/../../rounds.csv"),
      QCoreApplication::applicationDirPath() + QStringLiteral("/../../../rounds.csv"),
      QStringLiteral("rounds.csv"),
      QStringLiteral("various/ai/rounds.csv"),
      QStringLiteral("d:/cb26/cb26/rounds.csv"),
      QStringLiteral("d:/cb26/cb26/various/ai/rounds.csv"),
      QStringLiteral("d:/Containers/CB26b/rounds.csv")
  };
  for (const QString &p : candidateCsvs) {
    if (QFile::exists(p)) {
      roundsCsv = p;
      break;
    }
  }
  if (!roundsCsv.isEmpty()) {
    m_lapTimeChart->loadFromCsv(roundsCsv);
    if (m_lapTimeChart->roundCount() > 0) {
      appendLog(QStringLiteral(
                    "LapTimeChart: Loaded %1 historical round records from %2")
                    .arg(m_lapTimeChart->roundCount())
                    .arg(roundsCsv));
    }
  }

  // 5. Connect and start OBS Virtual Camera stream as default live feed
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
      QStringLiteral("Round completed: %1 (Round %2) | Lap Time: %3 s")
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

void MainWindow::onLapProgressChanged(int currentRound, double elapsedSec) {
  if (m_lapTimeChart) {
    m_lapTimeChart->setCurrentLapProgress(currentRound, elapsedSec);
  }
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

// ---------------------------------------------------------------------------
// Video Playback & Analysis Slots
// ---------------------------------------------------------------------------

void MainWindow::onOpenVideoClicked() {
  QString defaultDir = QStringLiteral("D:/Containers/testdata");
  if (!QDir(defaultDir).exists()) {
    defaultDir = QDir::currentPath();
  }

  QString filePath = QFileDialog::getOpenFileName(
      this, tr("Open Pre-Recorded Gameplay Video"), defaultDir,
      tr("Video Files (*.mp4 *.mkv *.avi *.mov *.webm);;All Files (*.*)"));

  if (!filePath.isEmpty()) {
    // Clear previous round records in lap time chart to analyze new run freshly
    m_lapTimeChart->clearData();
    if (m_videoFeed->openVideoFile(filePath)) {
      m_videoFeed->play();
      appendLog(QStringLiteral("Run Analysis Started: Loaded gameplay file %1")
                    .arg(filePath));
    }
  }
}

void MainWindow::onLoadTestGameplayClicked() {
  QString testFile = QStringLiteral("D:/Containers/testdata/pre-recorded-gameplay.mp4");
  if (!QFile::exists(testFile)) {
    testFile = QStringLiteral("pre-recorded-gameplay.mp4");
  }

  if (!QFile::exists(testFile)) {
    QMessageBox::warning(this, tr("Test Video Not Found"),
                         tr("Could not find test video at:\nD:\\Containers\\testdata\\pre-recorded-gameplay.mp4\n\nPlease use Open Video to locate the file."));
    onOpenVideoClicked();
    return;
  }

  // Clear previous run records for fresh analysis
  m_lapTimeChart->clearData();
  if (m_videoFeed->openVideoFile(testFile)) {
    m_videoFeed->play();
    appendLog(QStringLiteral("Run Analysis Started: Loaded test gameplay %1").arg(testFile));
  }
}

void MainWindow::onPlayVideoClicked() {
  if (!m_videoFeed->isVideoFileLoaded()) {
    onOpenVideoClicked();
    return;
  }
  m_videoFeed->play();
}

void MainWindow::onPauseVideoClicked() {
  m_videoFeed->pause();
}

void MainWindow::onStopVideoClicked() {
  m_videoFeed->stop();
}

void MainWindow::onLiveCameraClicked() {
  appendLog(QStringLiteral("Switching video feed back to OBS Virtual Camera (Live stream)..."));
  m_videoFeed->switchToLiveCamera();
}

void MainWindow::onSpeedChanged(int index) {
  qreal rate = 1.0;
  switch (index) {
  case 0: rate = 1.0; break;
  case 1: rate = 1.5; break;
  case 2: rate = 2.0; break;
  case 3: rate = 4.0; break;
  default: rate = 1.0; break;
  }
  m_videoFeed->setPlaybackRate(rate);
}

void MainWindow::onSliderSeek(int pos) {
  m_videoFeed->seek(pos);
}

void MainWindow::onVideoPositionChanged(qint64 posMs, qint64 durMs) {
  if (!m_userIsSeeking && m_timeSlider) {
    m_timeSlider->blockSignals(true);
    m_timeSlider->setValue(static_cast<int>(posMs));
    m_timeSlider->blockSignals(false);
  }

  if (m_timeLabel) {
    int curSec = static_cast<int>(posMs / 1000);
    int totSec = static_cast<int>(durMs / 1000);
    m_timeLabel->setText(QString::asprintf("%02d:%02d / %02d:%02d",
                                          curSec / 60, curSec % 60,
                                          totSec / 60, totSec % 60));
  }

  // Update live expanding lap time in LapTimeChart during video playback
  if (m_videoFeed && m_lapTimeChart) {
    int curRound = m_videoFeed->currentRound();
    if (curRound > 0) {
      m_lapTimeChart->setCurrentLapProgress(curRound, m_videoFeed->currentLapElapsedSec());
    }
  }
}

void MainWindow::onVideoDurationChanged(qint64 durMs) {
  if (m_timeSlider) {
    m_timeSlider->setRange(0, static_cast<int>(durMs));
  }
}

void MainWindow::onPlaybackStateChanged(bool isPlaying) {
  Q_UNUSED(isPlaying);
}

void MainWindow::onFeedModeChanged(bool isLiveCamera) {
  if (m_timeSlider) {
    m_timeSlider->setEnabled(!isLiveCamera);
  }
  if (m_speedCombo) {
    m_speedCombo->setEnabled(!isLiveCamera);
  }
  if (m_timeLabel) {
    m_timeLabel->setText(isLiveCamera ? QStringLiteral("LIVE FEED") : QStringLiteral("00:00 / 00:00"));
  }
  updateLiveFeedButtonState(isLiveCamera ? (m_videoFeed ? m_videoFeed->isStreamingActive() : true) : true);
}

void MainWindow::updateLiveFeedButtonState(bool streamingActive) {
  if (m_btnLiveFeed) {
    m_btnLiveFeed->setChecked(streamingActive);
    m_btnLiveFeed->setText(streamingActive ? QStringLiteral("Live Stream: ON")
                                           : QStringLiteral("Live Stream: OFF"));
  }
  if (m_btnVideoToggle) {
    m_btnVideoToggle->setChecked(streamingActive);
    m_btnVideoToggle->setText(streamingActive ? QStringLiteral("STREAM ON")
                                              : QStringLiteral("STREAM PAUSED"));
  }
}

// ---------------------------------------------------------------------------
// Database & Utility Slots
// ---------------------------------------------------------------------------

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
  bool active = m_videoFeed ? m_videoFeed->isStreamingActive() : true;
  bool newActive = !active;
  if (m_videoFeed) {
    m_videoFeed->setStreamingActive(newActive);
  }
  updateLiveFeedButtonState(newActive);
  appendLog(QStringLiteral("Live Video Stream: %1")
                .arg(newActive ? QStringLiteral("ON (Active Streaming)")
                               : QStringLiteral("OFF (Standby Mode)")));
}

void MainWindow::onPlotDataClicked() {
  appendLog(
      QStringLiteral("Plotting database query records into telemetry charts."));
  if (m_databaseMgr && m_lapTimeChart) {
    auto dbRounds = m_databaseMgr->loadRoundRecords();
    if (!dbRounds.isEmpty()) {
      for (const auto &r : dbRounds) {
        m_lapTimeChart->addRound(r.roundNumber, r.durationSec, r.timestamp);
      }
      appendLog(QStringLiteral("LapTimeChart: Plotted %1 round records from database.").arg(dbRounds.size()));
    } else {
      m_lapTimeChart->loadFromCsv(QStringLiteral("rounds.csv"));
    }
  }
  m_lapTimeChart->update();
}

void MainWindow::onSaveClicked() {
  appendLog(
      QStringLiteral("Telemetry configuration and database snapshot saved."));
  QMessageBox::information(this, tr("CoffeeBreak"),
                           tr("Configuration saved successfully."));
}

void MainWindow::onDisconnectClicked() { m_databaseMgr->disconnectDatabase(); }
