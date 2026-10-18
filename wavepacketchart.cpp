#include "wavepacketchart.h"

#include <QPainter>
#include <QPainterPath>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

WavepacketChart::WavepacketChart(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(250, 180);
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    connect(&m_timer, &QTimer::timeout, this, &WavepacketChart::onTick);
    m_elapsedTimer.start();
    m_timer.start(33); // ~30 fps
}

WavepacketChart::~WavepacketChart()
{
    m_timer.stop();
}

void WavepacketChart::onTick()
{
    double secs = m_elapsedTimer.elapsed() / 1000.0;
    m_phase = secs * 4.0;
    update();
}

void WavepacketChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    int w = width();
    int h = height();

    // High-tech dark telemetry canvas
    p.fillRect(rect(), QColor(13, 19, 31));

    const int ml = 40; // left
    const int mr = 15; // right
    const int mt = 15; // top
    const int mb = 30; // bottom

    int plotW = w - ml - mr;
    int plotH = h - mt - mb;

    if (plotW <= 10 || plotH <= 10) return;

    // Coordinate mapping
    const double minX = -17.0;
    const double maxX = 17.0;
    const double minY = -1.45;
    const double maxY = 1.65;

    auto toScreen = [&](double x, double y) -> QPointF {
        double rx = (x - minX) / (maxX - minX);
        double ry = (y - minY) / (maxY - minY);
        return QPointF(ml + rx * plotW, mt + plotH - ry * plotH);
    };

    // Grid lines
    QPen gridPen(QColor(30, 41, 59), 1, Qt::DotLine);
    p.setPen(gridPen);

    for (int gx = -15; gx <= 15; gx += 5) {
        QPointF topPt = toScreen(gx, maxY);
        QPointF botPt = toScreen(gx, minY);
        p.drawLine(topPt, botPt);
    }
    for (double gy = -1.0; gy <= 1.5; gy += 0.5) {
        QPointF leftPt = toScreen(minX, gy);
        QPointF rightPt = toScreen(maxX, gy);
        p.drawLine(leftPt, rightPt);
    }

    // Outer frame & axis
    QPen axisPen(QColor(51, 65, 85), 1, Qt::SolidLine);
    p.setPen(axisPen);
    p.drawRect(ml, mt, plotW, plotH);

    // Ticks & labels on X axis (-15, -10, -5, 0, 5, 10, 15)
    QFont font = p.font();
    font.setPointSize(7);
    p.setFont(font);
    p.setPen(QColor(148, 163, 184));

    for (int gx = -15; gx <= 15; gx += 5) {
        QPointF pt = toScreen(gx, minY);
        p.drawLine(pt.x(), pt.y(), pt.x(), pt.y() + 3);
        p.drawText(QRect(static_cast<int>(pt.x()) - 15, static_cast<int>(pt.y()) + 4, 30, 15), 
                   Qt::AlignCenter, QString::number(gx));
    }

    // Ticks & labels on Y axis (-1, -0.5, 0, 0.5, 1, 1.5)
    for (double gy = -1.0; gy <= 1.5; gy += 0.5) {
        QPointF pt = toScreen(minX, gy);
        p.drawLine(pt.x() - 3, pt.y(), pt.x(), pt.y());
        p.drawText(QRect(static_cast<int>(pt.x()) - 36, static_cast<int>(pt.y()) - 8, 32, 16), 
                   Qt::AlignRight | Qt::AlignVCenter, QString::number(gy, 'f', (std::abs(gy - std::round(gy)) < 0.01 ? 0 : 1)));
    }

    // Compute Wavepacket curve
    const int numSamples = 400;
    QPainterPath wavePath;
    double k = 3.0;

    auto waveFunc = [&](double x) -> double {
        return std::exp(-x * x / 20.0) * std::sin(k * x + m_phase);
    };

    for (int i = 0; i < numSamples; ++i) {
        double x = minX + (maxX - minX) * (i / static_cast<double>(numSamples - 1));
        double y = waveFunc(x);
        QPointF pt = toScreen(x, y);
        if (i == 0) {
            wavePath.moveTo(pt);
        } else {
            wavePath.lineTo(pt);
        }
    }

    // Draw wavepacket line (Cyan glow)
    QPen wavePen(QColor(6, 182, 212), 2.0, Qt::SolidLine);
    p.setPen(wavePen);
    p.drawPath(wavePath);

    // 1. Bracket at top with "Wavepacket"
    QPointF bLeft = toScreen(-7.5, 1.15);
    QPointF bRight = toScreen(7.5, 1.15);
    QPointF bMid = toScreen(0.0, 1.15);

    QPen bracketPen(QColor(129, 140, 248), 1.2, Qt::SolidLine);
    p.setPen(bracketPen);

    // Draw bracket
    p.drawLine(bLeft.x(), bLeft.y() + 6, bLeft.x(), bLeft.y());
    p.drawLine(bLeft.x(), bLeft.y(), bMid.x() - 4, bMid.y());
    p.drawLine(bMid.x() - 4, bMid.y(), bMid.x(), bMid.y() - 5);
    p.drawLine(bMid.x(), bMid.y() - 5, bMid.x() + 4, bMid.y());
    p.drawLine(bMid.x() + 4, bMid.y(), bRight.x(), bRight.y());
    p.drawLine(bRight.x(), bRight.y(), bRight.x(), bRight.y() + 6);

    font.setBold(true);
    font.setPointSize(8);
    p.setFont(font);
    p.setPen(QColor(248, 250, 252));
    p.drawText(QRect(static_cast<int>(bMid.x()) - 50, static_cast<int>(bMid.y()) - 22, 100, 16), 
               Qt::AlignCenter, QStringLiteral("Wavepacket"));

    // 2. Group velocity tracer
    double gx = 5.5;
    double gy = waveFunc(gx);
    QPointF gPt = toScreen(gx, gy);

    p.setBrush(QColor(56, 189, 248));
    p.setPen(QPen(Qt::white, 1));
    p.drawEllipse(gPt, 4.5, 4.5);

    // Group velocity label & arrow
    font.setBold(false);
    font.setPointSize(7);
    p.setFont(font);
    p.setPen(QColor(203, 213, 225));

    QPointF gTextPos = toScreen(8.5, 0.9);
    QRect gTextRect(static_cast<int>(gTextPos.x()), static_cast<int>(gTextPos.y()), 95, 32);
    p.drawText(gTextRect, Qt::AlignLeft | Qt::AlignTop, 
               QStringLiteral("Fixed positions in\nwave packet define\ngroup velocity vg"));

    // Arrow to blue dot
    QPainterPath gArrow;
    gArrow.moveTo(gTextRect.left() - 4, gTextRect.top() + 10);
    gArrow.quadTo(gTextRect.left() - 25, gTextRect.top() + 15, gPt.x() + 6, gPt.y() - 4);
    p.setPen(QPen(QColor(56, 189, 248), 1));
    p.drawPath(gArrow);
    p.drawLine(gPt.x() + 6, gPt.y() - 4, gPt.x() + 12, gPt.y() - 8);
    p.drawLine(gPt.x() + 6, gPt.y() - 4, gPt.x() + 8, gPt.y() - 12);

    // 3. Phase velocity tracer
    double px = std::fmod(-m_phase / k, 4.0 * M_PI / k);
    if (px < -6.0) px += 4.0 * M_PI / k;
    if (px > 6.0) px -= 4.0 * M_PI / k;
    px += 4.2;
    double py = waveFunc(px);
    QPointF pPt = toScreen(px, py);

    p.setBrush(QColor(16, 185, 129));
    p.setPen(QPen(Qt::white, 1));
    p.drawEllipse(pPt, 4.5, 4.5);

    // Phase velocity label & arrow
    QPointF pTextPos = toScreen(8.5, -0.6);
    QRect pTextRect(static_cast<int>(pTextPos.x()), static_cast<int>(pTextPos.y()), 95, 32);
    p.setPen(QColor(203, 213, 225));
    p.drawText(pTextRect, Qt::AlignLeft | Qt::AlignTop, 
               QStringLiteral("Points of fixed\nphase define\nphase velocity vp"));

    // Arrow to green dot
    QPainterPath pArrow;
    pArrow.moveTo(pTextRect.left() - 4, pTextRect.top() + 12);
    pArrow.quadTo(pTextRect.left() - 20, pTextRect.top() + 10, pPt.x() + 5, pPt.y() + 5);
    p.setPen(QPen(QColor(16, 185, 129), 1));
    p.drawPath(pArrow);
    p.drawLine(pPt.x() + 5, pPt.y() + 5, pPt.x() + 11, pPt.y() + 9);
    p.drawLine(pPt.x() + 5, pPt.y() + 5, pPt.x() + 7, pPt.y() + 13);

    // 4. Dispersion label with leftward arrow on the left side
    QPointF dTextPos = toScreen(-11.0, -0.5);
    QRect dTextRect(static_cast<int>(dTextPos.x()), static_cast<int>(dTextPos.y()), 80, 28);
    p.setPen(QColor(148, 163, 184));
    p.drawText(dTextRect, Qt::AlignLeft | Qt::AlignTop, QStringLiteral("Dispersion with\nvp < vg"));

    QPointF dArrowStart = toScreen(-7.5, -0.4);
    QPointF dArrowEnd = toScreen(-11.5, -0.2);
    p.setPen(QPen(QColor(148, 163, 184), 1));
    p.drawLine(dArrowStart, dArrowEnd);
    p.drawLine(dArrowEnd.x(), dArrowEnd.y(), dArrowEnd.x() + 6, dArrowEnd.y() - 4);
    p.drawLine(dArrowEnd.x(), dArrowEnd.y(), dArrowEnd.x() + 5, dArrowEnd.y() + 4);
}
