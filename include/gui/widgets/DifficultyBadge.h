#pragma once

#include <QLabel>
#include <QString>
#include "core/Difficulty.h"

// OOP Concept: Accessible UI Component
// Pill badge pairing semantic background colors with explicit non-color text labels (Easy/Medium/Hard).
class DifficultyBadge : public QLabel {
    Q_OBJECT

public:
    explicit DifficultyBadge(Difficulty diff, QWidget* parent = nullptr);
    void setDifficulty(Difficulty diff);
};
