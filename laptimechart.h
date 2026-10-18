#ifndef LAPTIMECHART_H
#define LAPTIMECHART_H

#include <QWidget>
#include <QDateTime>
#include <QTimer>
#include <vector>

struct RoundRecord {
    int roundNumber;
    double durationSec;
    QDateTime timestamp;
};

class LapTimeChart : public QWidget {
    Q_OBJECT

public:
    explicit LapTimeChart(QWidget *parent = nullptr);

    void addRound(int roundNumber, double durationSec, const QDateTime &timestamp = QDateTime::currentDateTime());
    void setActiveRound(int roundNumber);
    void setCurrentLapProgress(int activeRound, double currentLapElapsedSec);
    void loadFromCsv(const QString &csvPath);
    void clearData();

    int roundCount() const { return static_cast<int>(m_records.size()); }
    int activeRound() const { return m_activeRound; }
    double currentLapElapsedSec() const { return m_currentLapElapsedSec; }
    double lastDuration() const;
    double averageDuration() const;
    double fastestDuration() const;
    double totalDuration() const;

signals:
    void roundAdded(int roundNumber, double durationSec);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    std::vector<RoundRecord> m_records;
    double m_maxDuration = 60.0; // default scale
    int m_maxRound = 10;

    int m_activeRound = 0;
    double m_currentLapElapsedSec = 0.0;
    QTimer m_pulseTimer;
    int m_pulseStep = 0;
};

#endif // LAPTIMECHART_H
