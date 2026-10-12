#include "gui/widgets/ChartWidget.h"
#include "gui/ThemeTokens.h"
#include <QPainter>

ChartWidget::ChartWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(140);
}

void ChartWidget::setData(const std::map<Difficulty, int>& scoreByDiff, const std::map<Difficulty, int>& maxByDiff) {
    scoreByDiff_ = scoreByDiff;
    maxByDiff_ = maxByDiff;
    update();
}

void ChartWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    struct Level {
        QString name;
        Difficulty diff;
        QString color;
    };
    Level levels[] = {
        {"Easy", Difficulty::EASY, ThemeTokens::DifficultyEasy},
        {"Medium", Difficulty::MEDIUM, ThemeTokens::DifficultyMedium},
        {"Hard", Difficulty::HARD, ThemeTokens::DifficultyHard}
    };

    int barWidth = (w - 80) / 3;
    int maxBarHeight = h - 50;

    for (int i = 0; i < 3; ++i) {
        int x = 20 + i * (barWidth + 20);
        int score = scoreByDiff_.count(levels[i].diff) ? scoreByDiff_.at(levels[i].diff) : 0;
        int max = maxByDiff_.count(levels[i].diff) ? maxByDiff_.at(levels[i].diff) : 10;
        if (max == 0) max = 1;

        double ratio = static_cast<double>(score) / max;
        int barH = static_cast<int>(ratio * maxBarHeight);

        // Background track bar
        p.setBrush(QColor("#E2E8F0"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(x, 10, barWidth, maxBarHeight, 6, 6);

        // Filled value bar
        p.setBrush(QColor(levels[i].color));
        p.drawRoundedRect(x, 10 + (maxBarHeight - barH), barWidth, barH, 6, 6);

        // Text labels below
        p.setPen(QColor("#1E293B"));
        p.drawText(QRect(x, h - 35, barWidth, 30), Qt::AlignCenter,
            QString("%1\n%2/%3").arg(levels[i].name).arg(score).arg(max));
    }
}
