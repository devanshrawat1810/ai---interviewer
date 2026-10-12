#include "gui/widgets/FormField.h"
#include "gui/ThemeTokens.h"

FormField::FormField(const QString& labelText, QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    label_ = new QLabel(labelText, this);
    label_->setStyleSheet("font-weight: 600; font-size: 13px;");

    input_ = new QLineEdit(this);

    errorLabel_ = new QLabel(this);
    errorLabel_->setStyleSheet(QString("color: %1; font-size: 12px;").arg(ThemeTokens::ColorDanger));
    errorLabel_->setVisible(false);

    layout->addWidget(label_);
    layout->addWidget(input_);
    layout->addWidget(errorLabel_);
}

void FormField::setError(const QString& errorText) {
    if (errorText.isEmpty()) {
        clearError();
    } else {
        errorLabel_->setText(errorText);
        errorLabel_->setVisible(true);
        input_->setStyleSheet(QString("border: 1px solid %1;").arg(ThemeTokens::ColorDanger));
    }
}

void FormField::clearError() {
    errorLabel_->clear();
    errorLabel_->setVisible(false);
    input_->setStyleSheet("");
}
