#ifndef WAVEPACKETCHART_H
#define WAVEPACKETCHART_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>

class WavepacketChart : public QWidget {
    Q_OBJECT

public:
    explicit WavepacketChart(QWidget *parent = nullptr);
    ~WavepacketChart() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onTick();

private:
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    double m_phase = 0.0;
};

#endif // WAVEPACKETCHART_H
