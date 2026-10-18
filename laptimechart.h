#ifndef LAPTIMECHART_H
#define LAPTIMECHART_H

#include <QWidget>
#include <QDateTime>
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
    void loadFromCsv(const QString &csvPath);
    void clearData();

    int roundCount() const { return static_cast<int>(m_records.size()); }
    double lastDuration() const;
    double averageDuration() const;
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
};

#endif // LAPTIMECHART_H
