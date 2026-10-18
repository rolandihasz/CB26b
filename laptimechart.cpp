#include "laptimechart.h"

#include <QPainter>
#include <QPainterPath>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <cmath>
#include <algorithm>

LapTimeChart::LapTimeChart(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(250, 180);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void LapTimeChart::addRound(int roundNumber, double durationSec, const QDateTime &timestamp)
{
    RoundRecord rec;
    rec.roundNumber = roundNumber;
    rec.durationSec = durationSec;
    rec.timestamp = timestamp;

    m_records.push_back(rec);

    if (roundNumber > m_maxRound) {
        m_maxRound = roundNumber + 2;
    }
    if (durationSec > m_maxDuration) {
        m_maxDuration = durationSec * 1.2;
    }

    update();
    emit roundAdded(roundNumber, durationSec);
}

void LapTimeChart::loadFromCsv(const QString &csvPath)
{
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "LapTimeChart: Failed to open CSV:" << csvPath;
        return;
    }

    QTextStream in(&file);
    QString header = in.readLine(); // Skip header
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(',');
        if (parts.size() >= 2) {
            // Check if format is: Round,Timestamp,DurationMs
            // or: Time,Round
            if (parts.size() >= 3) {
                int rnd = parts[0].toInt();
                QDateTime dt = QDateTime::fromString(parts[1], Qt::ISODate);
                double durSec = parts[2].toDouble() / 1000.0;
                if (rnd > 0 && durSec > 0.0) {
                    addRound(rnd, durSec, dt);
                }
            } else if (parts.size() == 2) {
                // Time,Round
                QDateTime dt = QDateTime::fromString(parts[0], "yyyy-MM-dd HH:mm:ss");
                int rnd = parts[1].toInt();
                if (rnd > 0) {
                    addRound(rnd, 30.0 + rnd * 5.0, dt); // estimated if duration missing
                }
            }
        }
    }
    file.close();
    update();
}

void LapTimeChart::clearData()
{
    m_records.clear();
    m_maxRound = 10;
    m_maxDuration = 60.0;
    update();
}

double LapTimeChart::lastDuration() const
{
    if (m_records.empty()) return 0.0;
    return m_records.back().durationSec;
}

double LapTimeChart::averageDuration() const
{
    if (m_records.empty()) return 0.0;
    double sum = 0.0;
    for (const auto &r : m_records) {
        sum += r.durationSec;
    }
    return sum / m_records.size();
}

double LapTimeChart::totalDuration() const
{
    double sum = 0.0;
    for (const auto &r : m_records) {
        sum += r.durationSec;
    }
    return sum;
}

void LapTimeChart::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void LapTimeChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    int w = width();
    int h = height();

    // Background matching the screenshot style (clean white background with crisp grid)
    p.fillRect(rect(), Qt::white);

    // Margins for axes
    const int ml = 55; // left margin
    const int mr = 20; // right margin
    const int mt = 30; // top margin
    const int mb = 40; // bottom margin

    int plotW = w - ml - mr;
    int plotH = h - mt - mb;

    if (plotW <= 10 || plotH <= 10) return;

    // Grid lines
    QPen gridPen(QColor(220, 220, 220), 1, Qt::DotLine);
    p.setPen(gridPen);

    int numXSteps = 5;
    double xMax = std::max(5, m_maxRound);
    double xStep = xMax / numXSteps;

    for (int i = 0; i <= numXSteps; ++i) {
        int px = ml + static_cast<int>(i * (plotW / static_cast<double>(numXSteps)));
        p.drawLine(px, mt, px, mt + plotH);
    }

    int numYSteps = 5;
    double yMax = std::max(10.0, m_maxDuration);
    double yStep = yMax / numYSteps;

    for (int i = 0; i <= numYSteps; ++i) {
        int py = mt + plotH - static_cast<int>(i * (plotH / static_cast<double>(numYSteps)));
        p.drawLine(ml, py, ml + plotW, py);
    }

    // Outer frame & Axis lines
    QPen axisPen(Qt::black, 1, Qt::SolidLine);
    p.setPen(axisPen);
    p.drawRect(ml, mt, plotW, plotH);

    // X Axis ticks & numbers
    QFont font = p.font();
    font.setPointSize(8);
    p.setFont(font);

    for (int i = 0; i <= numXSteps; ++i) {
        int px = ml + static_cast<int>(i * (plotW / static_cast<double>(numXSteps)));
        int val = static_cast<int>(std::round(i * xStep));
        p.drawLine(px, mt + plotH, px, mt + plotH + 4);
        p.drawText(QRect(px - 20, mt + plotH + 5, 40, 20), Qt::AlignCenter, QString::number(val));
    }

    // Y Axis ticks & numbers
    for (int i = 0; i <= numYSteps; ++i) {
        int py = mt + plotH - static_cast<int>(i * (plotH / static_cast<double>(numYSteps)));
        double val = i * yStep;
        p.drawLine(ml - 4, py, ml, py);
        p.drawText(QRect(ml - 50, py - 10, 42, 20), Qt::AlignRight | Qt::AlignVCenter, QString::asprintf("%.0f", val));
    }

    // Title & Axis Labels
    font.setPointSize(9);
    font.setBold(true);
    p.setFont(font);
    p.setPen(Qt::black);
    p.drawText(QRect(ml, 5, plotW, 20), Qt::AlignCenter, QStringLiteral("Round Lap Time Expansion (s)"));

    font.setBold(false);
    font.setPointSize(8);
    p.setFont(font);
    p.drawText(QRect(ml, h - 22, plotW, 18), Qt::AlignCenter, QStringLiteral("Round Number (x)"));

    // Plot Data
    if (!m_records.empty()) {
        QPainterPath path;
        std::vector<QPointF> points;

        for (const auto &rec : m_records) {
            double rx = (rec.roundNumber / xMax) * plotW;
            double ry = (rec.durationSec / yMax) * plotH;
            double px = ml + rx;
            double py = mt + plotH - ry;
            points.emplace_back(px, py);
        }

        // Draw line
        if (!points.empty()) {
            path.moveTo(points[0]);
            for (size_t i = 1; i < points.size(); ++i) {
                path.lineTo(points[i]);
            }

            // Line pen (blue/cyan)
            QPen linePen(QColor(0, 102, 204), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            p.setPen(linePen);
            p.drawPath(path);

            // Points
            for (size_t i = 0; i < points.size(); ++i) {
                p.setBrush(QColor(255, 60, 60)); // Red marker
                p.setPen(QPen(Qt::black, 1));
                p.drawEllipse(points[i], 4, 4);

                // Draw duration label on point if points are not crowded
                if (points.size() < 25) {
                    font.setPointSize(7);
                    p.setFont(font);
                    p.setPen(Qt::black);
                    QString valStr = QString::asprintf("%.1fs", m_records[i].durationSec);
                    p.drawText(QRect(static_cast<int>(points[i].x()) - 25, static_cast<int>(points[i].y()) - 18, 50, 15), 
                               Qt::AlignCenter, valStr);
                }
            }
        }

        // Overlay Summary Badge (top right of plot area)
        QRect badgeRect(ml + plotW - 170, mt + 10, 160, 58);
        p.setBrush(QColor(245, 247, 250, 220));
        p.setPen(QPen(QColor(200, 205, 215), 1));
        p.drawRoundedRect(badgeRect, 4, 4);

        font.setPointSize(8);
        p.setFont(font);
        p.setPen(Qt::black);
        p.drawText(QRect(badgeRect.x() + 8, badgeRect.y() + 4, 144, 16), 
                   Qt::AlignLeft, QStringLiteral("Total Rounds: %1").arg(m_records.size()));
        p.drawText(QRect(badgeRect.x() + 8, badgeRect.y() + 20, 144, 16), 
                   Qt::AlignLeft, QStringLiteral("Last Lap: %1 s").arg(QString::number(lastDuration(), 'f', 1)));
        p.drawText(QRect(badgeRect.x() + 8, badgeRect.y() + 36, 144, 16), 
                   Qt::AlignLeft, QStringLiteral("Avg Lap: %1 s").arg(QString::number(averageDuration(), 'f', 1)));
    } else {
        // If no records yet, display instruction
        font.setPointSize(8);
        font.setItalic(true);
        p.setFont(font);
        p.setPen(Qt::black);
        p.drawText(QRect(ml, mt + plotH / 2 - 15, plotW, 30), Qt::AlignCenter, 
                   QStringLiteral("Waiting for round transitions from EagleEye AI..."));
    }
}
