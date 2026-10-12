#pragma once

#include <QWidget>
#include <map>
#include "core/Difficulty.h"

// OOP Concept: Custom Painted Component
// Draws interactive difficulty bar charts cleanly via QPainter without external chart modules.
class ChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChartWidget(QWidget* parent = nullptr);

    void setData(const std::map<Difficulty, int>& scoreByDiff, const std::map<Difficulty, int>& maxByDiff);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::map<Difficulty, int> scoreByDiff_;
    std::map<Difficulty, int> maxByDiff_;
};
