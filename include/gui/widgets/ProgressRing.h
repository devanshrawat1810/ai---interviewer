#pragma once

#include <QWidget>
#include <QPainter>
#include <QColor>

// OOP Concept: Custom Painted Control
// Circular timer draining smoothly via QPainter without external chart modules.
class ProgressRing : public QWidget {
    Q_OBJECT

public:
    explicit ProgressRing(QWidget* parent = nullptr);

    void setTime(int currentSeconds, int totalSeconds);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int currentSeconds_{60};
    int totalSeconds_{60};
};
