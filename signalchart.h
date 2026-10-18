#ifndef SIGNALCHART_H
#define SIGNALCHART_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <deque>

struct SignalPoint {
    double timeSec;
    double valGreen;
    double valRed;
};

class SignalChart : public QWidget {
    Q_OBJECT

public:
    explicit SignalChart(QWidget *parent = nullptr);
    ~SignalChart() override;

    void startStreaming();
    void stopStreaming();

    // Optionally feed custom telemetry into green/red channels
    void pushData(double valGreen, double valRed);

signals:
    void statsUpdated(int fps, int totalPoints);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onTick();

private:
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    std::deque<SignalPoint> m_buffer;

    int m_frameCount = 0;
    int m_lastFps = 33;
    qint64 m_lastFpsTime = 0;
    double m_windowDuration = 8.0; // 8 seconds visible on X axis
    double m_simPhase = 0.0;
};

#endif // SIGNALCHART_H
