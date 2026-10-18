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

    connect(&m_pulseTimer, &QTimer::timeout, this, [this]() {
        if (m_activeRound > 0) {
            m_pulseStep++;
            update();
        }
    });
    m_pulseTimer.start(100);
}

void LapTimeChart::setActiveRound(int roundNumber)
{
    if (m_activeRound != roundNumber) {
        m_activeRound = roundNumber;
        m_currentLapElapsedSec = 0.0;
        update();
    }
}

void LapTimeChart::setCurrentLapProgress(int activeRound, double currentLapElapsedSec)
{
    m_activeRound = activeRound;
    m_currentLapElapsedSec = std::max(0.0, currentLapElapsedSec);
    update();
}

void LapTimeChart::addRound(int roundNumber, double durationSec, const QDateTime &timestamp)
{
    for (auto &r : m_records) {
        if (r.roundNumber == roundNumber) {
            r.durationSec = durationSec;
            r.timestamp = timestamp;
            if (durationSec > m_maxDuration) {
                m_maxDuration = durationSec * 1.2;
            }
            update();
            emit roundAdded(roundNumber, durationSec);
            return;
        }
    }

    RoundRecord rec;
    rec.roundNumber = roundNumber;
    rec.durationSec = durationSec;
    rec.timestamp = timestamp;

    m_records.push_back(rec);

    if (roundNumber >= m_maxRound) {
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
            if (parts.size() >= 3) {
                int rnd = parts[0].toInt();
                QDateTime dt = QDateTime::fromString(parts[1], Qt::ISODate);
                double durSec = parts[2].toDouble() / 1000.0;
                if (rnd > 0 && durSec > 0.0) {
                    addRound(rnd, durSec, dt);
                }
            } else if (parts.size() == 2) {
                QDateTime dt = QDateTime::fromString(parts[0], "yyyy-MM-dd HH:mm:ss");
                int rnd = parts[1].toInt();
                if (rnd > 0) {
                    addRound(rnd, 30.0 + rnd * 5.0, dt);
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
    m_activeRound = 0;
    m_currentLapElapsedSec = 0.0;
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

double LapTimeChart::fastestDuration() const
{
    if (m_records.empty()) return 0.0;
    double minD = m_records[0].durationSec;
    for (const auto &r : m_records) {
        if (r.durationSec < minD) minD = r.durationSec;
    }
    return minD;
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

    // High-tech dark telemetry canvas
    p.fillRect(rect(), QColor(13, 19, 31));

    // Margins for axes
    const int ml = 55; // left margin
    const int mr = 20; // right margin
    const int mt = 30; // top margin
    const int mb = 40; // bottom margin

    int plotW = w - ml - mr;
    int plotH = h - mt - mb;

    if (plotW <= 10 || plotH <= 10) return;

    // Calculate smart X-axis bounds (minRound and maxRound)
    int minRound = 999999;
    int maxRound = 0;
    for (const auto &r : m_records) {
        if (r.roundNumber < minRound) minRound = r.roundNumber;
        if (r.roundNumber > maxRound) maxRound = r.roundNumber;
    }
    if (m_activeRound > 0) {
        if (m_activeRound < minRound) minRound = m_activeRound;
        if (m_activeRound > maxRound) maxRound = m_activeRound;
    }

    int xMin = 0;
    int xMax = 10;
    if (minRound <= maxRound && maxRound > 0) {
        if (minRound <= 10) {
            xMin = 0;
            xMax = std::max(10, maxRound + 2);
        } else {
            xMin = std::max(0, minRound - 2);
            xMax = maxRound + 3;
        }
    }
    if (xMax <= xMin) xMax = xMin + 5;

    // Calculate smart Y-axis bounds (duration in seconds)
    double maxDur = 30.0;
    for (const auto &r : m_records) {
        if (r.durationSec > maxDur) maxDur = r.durationSec;
    }
    if (m_activeRound > 0 && m_currentLapElapsedSec > maxDur) {
        maxDur = m_currentLapElapsedSec;
    }
    double yMax = std::max(40.0, maxDur * 1.25);

    auto mapX = [&](double rnd) -> double {
        return ml + ((rnd - xMin) / static_cast<double>(xMax - xMin)) * plotW;
    };
    auto mapY = [&](double dur) -> double {
        return mt + plotH - (dur / yMax) * plotH;
    };

    // Grid lines
    QPen gridPen(QColor(30, 41, 59), 1, Qt::DotLine);
    p.setPen(gridPen);

    int numXSteps = 5;
    double xStep = (xMax - xMin) / static_cast<double>(numXSteps);

    for (int i = 0; i <= numXSteps; ++i) {
        int px = ml + static_cast<int>(i * (plotW / static_cast<double>(numXSteps)));
        p.drawLine(px, mt, px, mt + plotH);
    }

    int numYSteps = 5;
    double yStep = yMax / numYSteps;

    for (int i = 0; i <= numYSteps; ++i) {
        int py = mt + plotH - static_cast<int>(i * (plotH / static_cast<double>(numYSteps)));
        p.drawLine(ml, py, ml + plotW, py);
    }

    // Outer frame & Axis lines
    QPen axisPen(QColor(51, 65, 85), 1, Qt::SolidLine);
    p.setPen(axisPen);
    p.drawRect(ml, mt, plotW, plotH);

    // X Axis ticks & numbers
    QFont font = p.font();
    font.setPointSize(8);
    p.setFont(font);
    p.setPen(QColor(148, 163, 184));

    for (int i = 0; i <= numXSteps; ++i) {
        int px = ml + static_cast<int>(i * (plotW / static_cast<double>(numXSteps)));
        int val = static_cast<int>(std::round(xMin + i * xStep));
        p.drawLine(px, mt + plotH, px, mt + plotH + 4);
        p.drawText(QRect(px - 25, mt + plotH + 5, 50, 20), Qt::AlignCenter, QString::number(val));
    }

    // Y Axis ticks & numbers
    for (int i = 0; i <= numYSteps; ++i) {
        int py = mt + plotH - static_cast<int>(i * (plotH / static_cast<double>(numYSteps)));
        double val = i * yStep;
        p.drawLine(ml - 4, py, ml, py);
        p.drawText(QRect(ml - 50, py - 10, 42, 20), Qt::AlignRight | Qt::AlignVCenter, QString::asprintf("%.0f", val));
    }

    // Title & Axis Labels
    QFont titleFont = font;
    titleFont.setPointSize(9);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.setPen(QColor(248, 250, 252));
    p.drawText(QRect(ml, 5, plotW, 20), Qt::AlignCenter, QStringLiteral("Round Lap Time Expansion (s)"));

    p.setFont(font);
    p.setPen(QColor(148, 163, 184));
    p.drawText(QRect(ml, h - 22, plotW, 18), Qt::AlignCenter, QStringLiteral("Round Number (x)"));

    // Plot Data
    std::vector<QPointF> points;
    for (const auto &rec : m_records) {
        points.emplace_back(mapX(rec.roundNumber), mapY(rec.durationSec));
    }

    // 1. Draw gradient fill & line for completed rounds
    if (points.size() >= 2) {
        QPainterPath fillPath;
        fillPath.moveTo(points[0].x(), mt + plotH);
        for (const auto &pt : points) {
            fillPath.lineTo(pt);
        }
        fillPath.lineTo(points.back().x(), mt + plotH);
        fillPath.closeSubpath();

        QLinearGradient fillGrad(0, mt, 0, mt + plotH);
        fillGrad.setColorAt(0.0, QColor(56, 189, 248, 55));
        fillGrad.setColorAt(1.0, QColor(56, 189, 248, 0));
        p.fillPath(fillPath, fillGrad);

        QPainterPath linePath;
        linePath.moveTo(points[0]);
        for (size_t i = 1; i < points.size(); ++i) {
            linePath.lineTo(points[i]);
        }
        QPen linePen(QColor(56, 189, 248), 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(linePen);
        p.drawPath(linePath);
    }

    // 2. Draw nodes for completed rounds
    for (size_t i = 0; i < points.size(); ++i) {
        p.setBrush(QColor(16, 185, 129)); // Emerald marker
        p.setPen(QPen(Qt::white, 1.5));
        p.drawEllipse(points[i], 4.5, 4.5);

        if (points.size() < 25) {
            QFont fSmall = font;
            fSmall.setPointSize(7);
            p.setFont(fSmall);
            p.setPen(QColor(226, 232, 240));
            QString valStr = QString::asprintf("%.1fs", m_records[i].durationSec);
            p.drawText(QRect(static_cast<int>(points[i].x()) - 25, static_cast<int>(points[i].y()) - 18, 50, 15), 
                       Qt::AlignCenter, valStr);
        }
    }

    // 3. Draw active in-progress expanding round
    if (m_activeRound > 0) {
        double activeX = mapX(m_activeRound);
        double activeY = mapY(m_currentLapElapsedSec);
        QPointF activePt(activeX, activeY);

        if (!points.empty()) {
            QPainterPath dashPath;
            dashPath.moveTo(points.back());
            dashPath.lineTo(activePt);
            QPen dashPen(QColor(245, 158, 11, 200), 2, Qt::DashLine, Qt::RoundCap);
            p.setPen(dashPen);
            p.drawPath(dashPath);
        }

        // Pulsing halo beacon
        double pulseRadius = 5.0 + 3.0 * std::sin(m_pulseStep * 0.35);
        p.setBrush(QColor(245, 158, 11, 70));
        p.setPen(Qt::NoPen);
        p.drawEllipse(activePt, pulseRadius, pulseRadius);

        // Core bright beacon
        p.setBrush(QColor(251, 191, 36));
        p.setPen(QPen(Qt::white, 1.5));
        p.drawEllipse(activePt, 4.5, 4.5);

        // Live expanding tag
        int mm = static_cast<int>(m_currentLapElapsedSec) / 60;
        int ss = static_cast<int>(m_currentLapElapsedSec) % 60;
        int ms = static_cast<int>((m_currentLapElapsedSec - static_cast<int>(m_currentLapElapsedSec)) * 10);
        QString liveTag = QString::asprintf("R%d: %02d:%02d.%d (ACTIVE)", m_activeRound, mm, ss, ms);

        QFont fLive = font;
        fLive.setPointSize(8);
        fLive.setBold(true);
        p.setFont(fLive);
        p.setPen(QColor(251, 191, 36));
        p.drawText(QRect(static_cast<int>(activeX) - 75, static_cast<int>(activeY) - 22, 150, 18),
                   Qt::AlignCenter, liveTag);
    }

    // 4. If empty and no active round
    if (m_records.empty() && m_activeRound == 0) {
        QFont fItalic = font;
        fItalic.setItalic(true);
        p.setFont(fItalic);
        p.setPen(QColor(148, 163, 184));
        p.drawText(QRect(ml, mt + plotH / 2 - 15, plotW, 30), Qt::AlignCenter, 
                   QStringLiteral("Waiting for EagleEye AI round detection or video playback..."));
    }

    // 5. Overlay Summary Badge (top right of plot area)
    int badgeH = (m_activeRound > 0) ? 76 : 60;
    QRect badgeRect(ml + plotW - 190, mt + 10, 180, badgeH);
    p.setBrush(QColor(15, 23, 42, 235));
    p.setPen(QPen(QColor(51, 65, 85), 1));
    p.drawRoundedRect(badgeRect, 5, 5);

    QFont badgeFont = font;
    badgeFont.setPointSize(8);
    p.setFont(badgeFont);

    int by = badgeRect.y() + 6;
    if (m_activeRound > 0) {
        p.setPen(QColor(251, 191, 36)); // Amber
        int mm = static_cast<int>(m_currentLapElapsedSec) / 60;
        int ss = static_cast<int>(m_currentLapElapsedSec) % 60;
        p.drawText(QRect(badgeRect.x() + 8, by, 164, 16), Qt::AlignLeft,
                   QString::asprintf("ACTIVE R%d: %02d:%02d (EXPANDING)", m_activeRound, mm, ss));
        by += 16;
    }

    p.setPen(QColor(226, 232, 240));
    p.drawText(QRect(badgeRect.x() + 8, by, 164, 16), Qt::AlignLeft,
               QStringLiteral("Completed Rounds: %1").arg(m_records.size()));
    by += 16;

    p.setPen(QColor(56, 189, 248)); // Electric cyan
    p.drawText(QRect(badgeRect.x() + 8, by, 164, 16), Qt::AlignLeft,
               QStringLiteral("Last Lap: %1 s").arg(QString::number(lastDuration(), 'f', 1)));
    by += 16;

    p.setPen(QColor(52, 211, 153)); // Emerald
    p.drawText(QRect(badgeRect.x() + 8, by, 164, 16), Qt::AlignLeft,
               QStringLiteral("Avg Lap: %1 s").arg(QString::number(averageDuration(), 'f', 1)));
}
