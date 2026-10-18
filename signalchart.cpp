#include "signalchart.h"

#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <cmath>
#include <cstdlib>

SignalChart::SignalChart(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(250, 180);
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    connect(&m_timer, &QTimer::timeout, this, &SignalChart::onTick);
    m_elapsedTimer.start();
    m_timer.start(33); // ~30 fps
}

SignalChart::~SignalChart()
{
    m_timer.stop();
}

void SignalChart::startStreaming()
{
    if (!m_timer.isActive()) {
        m_timer.start(33);
    }
}

void SignalChart::stopStreaming()
{
    m_timer.stop();
}

void SignalChart::pushData(double valGreen, double valRed)
{
    double nowSec = m_elapsedTimer.elapsed() / 1000.0;
    m_buffer.push_back({nowSec, valGreen, valRed});

    // Discard points older than window duration
    while (!m_buffer.empty() && (nowSec - m_buffer.front().timeSec) > (m_windowDuration + 1.0)) {
        m_buffer.pop_front();
    }
    update();
}

void SignalChart::onTick()
{
    double key = m_elapsedTimer.elapsed() / 1000.0;
    
    // Generate simulated signal matching legacy formula
    double g = std::sin(key) + (static_cast<double>(std::rand()) / RAND_MAX) * 0.3 * std::sin(key / 0.3843);
    double r = std::cos(key) + (static_cast<double>(std::rand()) / RAND_MAX) * 0.25 * std::sin(key / 0.4364);

    // Clamp into reasonable plot range
    g = std::max(-1.2, std::min(1.2, g));
    r = std::max(-1.2, std::min(1.2, r));

    pushData(g, r);

    m_frameCount++;
    qint64 nowMs = m_elapsedTimer.elapsed();
    if (nowMs - m_lastFpsTime >= 1000) {
        m_lastFps = static_cast<int>(m_frameCount * 1000.0 / (nowMs - m_lastFpsTime));
        m_frameCount = 0;
        m_lastFpsTime = nowMs;
        emit statsUpdated(m_lastFps, static_cast<int>(m_buffer.size() * 2));
    }
}

void SignalChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    int w = width();
    int h = height();

    // High-tech dark telemetry canvas
    p.fillRect(rect(), QColor(13, 19, 31));

    const int ml = 45; // left margin
    const int mr = 15; // right margin
    const int mt = 15; // top margin
    const int mb = 30; // bottom margin

    int plotW = w - ml - mr;
    int plotH = h - mt - mb;

    if (plotW <= 10 || plotH <= 10) return;

    // Grid lines
    QPen gridPen(QColor(30, 41, 59), 1, Qt::DotLine);
    p.setPen(gridPen);

    int numYSteps = 4; // -1.0, -0.5, 0.0, 0.5, 1.0
    for (int i = 0; i <= numYSteps; ++i) {
        int py = mt + static_cast<int>(i * (plotH / static_cast<double>(numYSteps)));
        p.drawLine(ml, py, ml + plotW, py);
    }

    int numXSteps = 4;
    for (int i = 0; i <= numXSteps; ++i) {
        int px = ml + static_cast<int>(i * (plotW / static_cast<double>(numXSteps)));
        p.drawLine(px, mt, px, mt + plotH);
    }

    // Outer frame & axis
    QPen axisPen(QColor(51, 65, 85), 1, Qt::SolidLine);
    p.setPen(axisPen);
    p.drawRect(ml, mt, plotW, plotH);

    // Y Axis labels: 1, 0.5, 0, -0.5, -1
    QFont font = p.font();
    font.setPointSize(7);
    p.setFont(font);
    p.setPen(QColor(148, 163, 184));

    double yLabels[] = {1.0, 0.5, 0.0, -0.5, -1.0};
    for (int i = 0; i <= numYSteps; ++i) {
        int py = mt + static_cast<int>(i * (plotH / static_cast<double>(numYSteps)));
        p.drawLine(ml - 3, py, ml, py);
        p.drawText(QRect(ml - 42, py - 8, 38, 16), Qt::AlignRight | Qt::AlignVCenter, 
                   QString::number(yLabels[i], 'f', (i % 2 == 0 ? 0 : 1)));
    }

    // X Axis time labels
    double currentSec = m_elapsedTimer.elapsed() / 1000.0;
    double startSec = currentSec - m_windowDuration;

    for (int i = 0; i <= numXSteps; ++i) {
        int px = ml + static_cast<int>(i * (plotW / static_cast<double>(numXSteps)));
        p.drawLine(px, mt + plotH, px, mt + plotH + 3);

        double tickTimeSec = startSec + i * (m_windowDuration / numXSteps);
        QTime tickTime = QTime::currentTime().addMSecs(static_cast<int>((tickTimeSec - currentSec) * 1000.0));
        QString timeStr = tickTime.toString("HH:mm:ss");

        p.drawText(QRect(px - 30, mt + plotH + 4, 60, 16), Qt::AlignCenter, timeStr);
    }

    // Plot green and red lines
    if (m_buffer.size() >= 2) {
        QPainterPath pathGreen;
        QPainterPath pathRed;

        auto toPoint = [&](double timeSec, double val) -> QPointF {
            double rx = (timeSec - startSec) / m_windowDuration;
            double ry = (val - (-1.2)) / 2.4; // range [-1.2, 1.2]
            double px = ml + rx * plotW;
            double py = mt + plotH - ry * plotH;
            return QPointF(px, py);
        };

        bool first = true;
        for (const auto &pt : m_buffer) {
            if (pt.timeSec < startSec) continue;
            QPointF pg = toPoint(pt.timeSec, pt.valGreen);
            QPointF pr = toPoint(pt.timeSec, pt.valRed);

            if (first) {
                pathGreen.moveTo(pg);
                pathRed.moveTo(pr);
                first = false;
            } else {
                pathGreen.lineTo(pg);
                pathRed.lineTo(pr);
            }
        }

        // Green line (GPU Util - Neon Emerald)
        QPen penGreen(QColor(16, 185, 129), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(penGreen);
        p.drawPath(pathGreen);

        // Red line (GPU Temp - Neon Coral)
        QPen penRed(QColor(244, 63, 94), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(penRed);
        p.drawPath(pathRed);
    }

    // FPS / data points overlay
    font.setPointSize(7);
    p.setFont(font);
    p.setPen(QColor(56, 189, 248)); // Cyan
    QString infoStr = QStringLiteral("%1 FPS, Total Points: %2")
                          .arg(m_lastFps)
                          .arg(m_buffer.size() * 2);
    p.drawText(QRect(ml + 6, mt + 4, 180, 16), Qt::AlignLeft, infoStr);
}
