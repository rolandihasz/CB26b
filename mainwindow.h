#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QTabWidget>

#include "databasemanager.h"
#include "eagleeye_classifier.h"
#include "laptimechart.h"
#include "signalchart.h"
#include "telemetrymanager.h"
#include "videofeedwidget.h"
#include "wavepacketchart.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private slots:
  void appendLog(const QString &message);
  void onRoundDetected(int roundNumber, const QString &roundStr,
                       double durationSec);
  void onGpuUpdated(const GpuStats &stats);
  void onIpUpdated(const QString &intIp, const QString &extIp);

  // Video playback & analysis toolbar slots
  void onOpenVideoClicked();
  void onPlayVideoClicked();
  void onPauseVideoClicked();
  void onStopVideoClicked();
  void onLiveCameraClicked();
  void onSpeedChanged(int index);
  void onSliderSeek(int pos);
  void onVideoPositionChanged(qint64 posMs, qint64 durMs);
  void onVideoDurationChanged(qint64 durMs);
  void onPlaybackStateChanged(bool isPlaying);
  void onFeedModeChanged(bool isLiveCamera);
  void onLoadTestGameplayClicked();

  // Database Tab & existing action slots
  void onSetIterationClicked();
  void onRunQueryClicked();
  void onRequestDataClicked();
  void onRefreshClicked();
  void onDropDbTableClicked();
  void onClearLogClicked();
  void onExitClicked();

  void onHelpDeskClicked();
  void onMyAccountClicked();
  void onLiveFeedClicked();
  void onPlotDataClicked();
  void onSaveClicked();
  void onDisconnectClicked();

private:
  void setupUiCustom();
  void setupConnections();
  void initializeSystem();

  Ui::MainWindow *ui;

  EagleEyeClassifier m_classifier;
  VideoFeedWidget *m_videoFeed = nullptr;
  SignalChart *m_signalChart = nullptr;
  WavepacketChart *m_wavepacketChart = nullptr;
  LapTimeChart *m_lapTimeChart = nullptr;

  TelemetryManager *m_telemetryMgr = nullptr;
  DatabaseManager *m_databaseMgr = nullptr;

  // Video Playback UI Toolbar Controls
  QSlider *m_timeSlider = nullptr;
  QLabel *m_timeLabel = nullptr;
  QComboBox *m_speedCombo = nullptr;
  QPushButton *m_quickTestBtn = nullptr;
  bool m_userIsSeeking = false;

  // UI Widgets
  QPlainTextEdit *m_eventLogView = nullptr;
  QLineEdit *m_internalIpEdit = nullptr;
  QLineEdit *m_externalIpEdit = nullptr;
  QLabel *m_statusLabel = nullptr;
  QProgressBar *m_progressBar = nullptr;
  QLineEdit *m_iterationEdit = nullptr;
  QLabel *m_gpuStatsLabel = nullptr;
  QLabel *m_altitudeLabel = nullptr;
  QLabel *m_positionLabel = nullptr;
};

#endif // MAINWINDOW_H
