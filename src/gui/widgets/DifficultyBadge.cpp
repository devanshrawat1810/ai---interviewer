#include "gui/widgets/DifficultyBadge.h"
#include "gui/ThemeTokens.h"

DifficultyBadge::DifficultyBadge(Difficulty diff, QWidget* parent) : QLabel(parent) {
    setAlignment(Qt::AlignCenter);
    setFixedHeight(26);
    setDifficulty(diff);
}

void DifficultyBadge::setDifficulty(Difficulty diff) {
    QString text;
    QString color;
    switch (diff) {
        case Difficulty::EASY:
            text = "Easy";
            color = ThemeTokens::DifficultyEasy;
            break;
        case Difficulty::MEDIUM:
            text = "Medium";
            color = ThemeTokens::DifficultyMedium;
            break;
        case Difficulty::HARD:
            text = "Hard";
            color = ThemeTokens::DifficultyHard;
            break;
    }
    setText(text);
    setStyleSheet(QString(R"(
        QLabel {
            background-color: %1;
            color: #FFFFFF;
            font-weight: bold;
            font-size: 11px;
            padding: 2px 10px;
            border-radius: 12px;
        }
    )").arg(color));
}
