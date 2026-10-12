#include "gui/widgets/Card.h"
#include "gui/ThemeTokens.h"

Card::Card(QWidget* parent) : QFrame(parent) {
    setFrameShape(QFrame::StyledPanel);
    setStyleSheet(QString(R"(
        Card {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 12px;
            padding: 16px;
        }
    )").arg(ThemeTokens::LightSurface, ThemeTokens::LightBorder));
}
