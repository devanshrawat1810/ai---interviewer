#include "gui/widgets/CodingQuestionWidget.h"
#include "gui/ThemeTokens.h"
#include <QFontDatabase>

CodingQuestionWidget::CodingQuestionWidget(const QString& starterCode, QWidget* parent)
    : BaseQuestionWidget(parent), starterCode_(starterCode) {
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(10);

    editor_ = new QPlainTextEdit(this);
    editor_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    editor_->setPlainText(starterCode_);
    editor_->setMinimumHeight(240);
    editor_->setStyleSheet("background-color: #1E293B; color: #F8FAFC; border-radius: 8px; padding: 10px;");

    highlighter_ = new CppSyntaxHighlighter(editor_->document());

    // Controls toolbar
    auto* bar = new QWidget(this);
    auto* barLayout = new QHBoxLayout(bar);
    barLayout->setContentsMargins(0, 0, 0, 0);

    runBtn_ = new QPushButton("Run Tests", bar);
    runBtn_->setCursor(Qt::PointingHandCursor);
    runBtn_->setStyleSheet(QString("background-color: %1; color: white; border-radius: 6px; padding: 6px 14px; font-weight: bold;")
        .arg(ThemeTokens::ColorAccent));

    resetBtn_ = new QPushButton("Reset Template", bar);
    resetBtn_->setCursor(Qt::PointingHandCursor);
    resetBtn_->setStyleSheet("background-color: transparent; border: 1px solid #CBD5E1; border-radius: 6px; padding: 6px 12px;");

    barLayout->addWidget(runBtn_);
    barLayout->addWidget(resetBtn_);
    barLayout->addStretch();

    outputLabel_ = new QLabel("Ready to test. Press 'Run Tests' to compile and execute.", this);
    outputLabel_->setStyleSheet("background-color: #F1F5F9; border-radius: 6px; padding: 8px 12px; font-size: 13px; color: #334155;");

    connect(runBtn_, &QPushButton::clicked, this, [this]() {
        outputLabel_->setText("Compiling and running tests on worker thread...");
        emit runRequested(editor_->toPlainText());
    });

    connect(resetBtn_, &QPushButton::clicked, this, [this]() {
        editor_->setPlainText(starterCode_);
        outputLabel_->setText("Template reset.");
    });

    layout->addWidget(editor_);
    layout->addWidget(bar);
    layout->addWidget(outputLabel_);
}

QString CodingQuestionWidget::getAnswer() const {
    return editor_->toPlainText();
}

void CodingQuestionWidget::clearAnswer() {
    editor_->setPlainText(starterCode_);
}

void CodingQuestionWidget::setTestResults(const QString& resultsText, bool passed) {
    outputLabel_->setText(resultsText);
    QString col = passed ? ThemeTokens::ColorSuccess : ThemeTokens::ColorDanger;
    outputLabel_->setStyleSheet(QString("background-color: #F8FAFC; border: 1px solid %1; border-radius: 6px; padding: 8px 12px; font-size: 13px; color: %1;")
        .arg(col));
}
