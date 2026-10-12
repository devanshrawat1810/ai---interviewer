#include "gui/widgets/PrimaryButton.h"
#include "gui/ThemeTokens.h"

PrimaryButton::PrimaryButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent), originalText_(text) {
    setCursor(Qt::PointingHandCursor);
    setMinimumHeight(42);
    setStyleSheet(QString(R"(
        QPushButton {
            background-color: %1;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            font-weight: 600;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: %2;
        }
        QPushButton:disabled {
            background-color: #94A3B8;
            color: #E2E8F0;
        }
    )").arg(ThemeTokens::ColorPrimary, ThemeTokens::ColorPrimaryHover));
}

void PrimaryButton::setLoading(bool loading) {
    isLoading_ = loading;
    setEnabled(!loading);
    if (loading) {
        setText("Loading...");
    } else {
        setText(originalText_);
    }
}
