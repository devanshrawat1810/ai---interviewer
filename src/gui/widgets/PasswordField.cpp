#include "gui/widgets/PasswordField.h"
#include "gui/ThemeTokens.h"

PasswordField::PasswordField(const QString& labelText, QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    label_ = new QLabel(labelText, this);
    label_->setStyleSheet("font-weight: 600; font-size: 13px;");

    auto* row = new QWidget(this);
    auto* rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(6);

    input_ = new QLineEdit(row);
    input_->setEchoMode(QLineEdit::Password);

    toggleBtn_ = new QPushButton("Show", row);
    toggleBtn_->setCursor(Qt::PointingHandCursor);
    toggleBtn_->setFixedWidth(60);
    toggleBtn_->setStyleSheet("padding: 6px; font-size: 12px; background: transparent; border: 1px solid #CBD5E1;");

    connect(toggleBtn_, &QPushButton::clicked, this, [this]() {
        isVisible_ = !isVisible_;
        input_->setEchoMode(isVisible_ ? QLineEdit::Normal : QLineEdit::Password);
        toggleBtn_->setText(isVisible_ ? "Hide" : "Show");
    });

    rowLayout->addWidget(input_);
    rowLayout->addWidget(toggleBtn_);

    errorLabel_ = new QLabel(this);
    errorLabel_->setStyleSheet(QString("color: %1; font-size: 12px;").arg(ThemeTokens::ColorDanger));
    errorLabel_->setVisible(false);

    layout->addWidget(label_);
    layout->addWidget(row);
    layout->addWidget(errorLabel_);
}

void PasswordField::setError(const QString& errorText) {
    if (errorText.isEmpty()) {
        clearError();
    } else {
        errorLabel_->setText(errorText);
        errorLabel_->setVisible(true);
        input_->setStyleSheet(QString("border: 1px solid %1;").arg(ThemeTokens::ColorDanger));
    }
}

void PasswordField::clearError() {
    errorLabel_->clear();
    errorLabel_->setVisible(false);
    input_->setStyleSheet("");
}
