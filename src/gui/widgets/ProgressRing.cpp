#include "gui/widgets/ProgressRing.h"
#include "gui/ThemeTokens.h"
#include <QFont>

ProgressRing::ProgressRing(QWidget* parent) : QWidget(parent) {
    setFixedSize(64, 64);
}

void ProgressRing::setTime(int currentSeconds, int totalSeconds) {
    currentSeconds_ = currentSeconds;
    totalSeconds_ = (totalSeconds > 0) ? totalSeconds : 60;
    update();
}

void ProgressRing::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF rect(4, 4, width() - 8, height() - 8);

    // Background track
    QPen trackPen(QColor(ThemeTokens::LightBorder), 5);
    p.setPen(trackPen);
    p.drawArc(rect, 0, 360 * 16);

    // Progress arc
    double frac = (totalSeconds_ > 0) ? static_cast<double>(currentSeconds_) / totalSeconds_ : 0.0;
    int spanAngle = static_cast<int>(frac * 360.0 * 16.0);

    QColor ringColor = QColor(ThemeTokens::ColorPrimary);
    if (frac <= 0.10) {
        ringColor = QColor(ThemeTokens::ColorDanger);
    } else if (frac <= 0.30) {
        ringColor = QColor(ThemeTokens::ColorWarning);
    }

    QPen arcPen(ringColor, 5, Qt::SolidLine, Qt::RoundCap);
    p.setPen(arcPen);
    p.drawArc(rect, 90 * 16, -spanAngle);

    // Text in center (e.g. "45s")
    p.setPen(QColor(ThemeTokens::LightTextPrimary));
    QFont font = p.font();
    font.setPointSize(10);
    font.setBold(true);
    p.setFont(font);
    p.drawText(rect, Qt::AlignCenter, QString::number(currentSeconds_) + "s");
}
