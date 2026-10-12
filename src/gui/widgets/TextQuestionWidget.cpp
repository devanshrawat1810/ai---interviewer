#include "gui/widgets/TextQuestionWidget.h"
#include <QStringList>
#include <QRegularExpression>

TextQuestionWidget::TextQuestionWidget(QWidget* parent) : BaseQuestionWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(8);

    editor_ = new QTextEdit(this);
    editor_->setPlaceholderText("Type your detailed explanation here. Be clear and structured...");
    editor_->setMinimumHeight(180);

    counterLabel_ = new QLabel("0 words | 0 characters", this);
    counterLabel_->setStyleSheet("color: #64748B; font-size: 12px;");
    counterLabel_->setAlignment(Qt::AlignRight);

    connect(editor_, &QTextEdit::textChanged, this, &TextQuestionWidget::updateCounts);

    layout->addWidget(editor_);
    layout->addWidget(counterLabel_);
}

QString TextQuestionWidget::getAnswer() const {
    return editor_->toPlainText().trimmed();
}

void TextQuestionWidget::clearAnswer() {
    editor_->clear();
}

void TextQuestionWidget::updateCounts() {
    QString text = editor_->toPlainText();
    int chars = text.length();
    QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    counterLabel_->setText(QString("%1 words | %2 characters").arg(words.size()).arg(chars));
}
