#pragma once

#include "gui/widgets/QuestionViewFactory.h"
#include "gui/widgets/CppSyntaxHighlighter.h"
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

// OOP Concept: Concrete View Specialization & Subprocess Integration
// Split code editor with C++ syntax highlighting, line numbers, and off-thread execution test runner.
class CodingQuestionWidget : public BaseQuestionWidget {
    Q_OBJECT

public:
    explicit CodingQuestionWidget(const QString& starterCode, QWidget* parent = nullptr);

    QString getAnswer() const override;
    void clearAnswer() override;

signals:
    void runRequested(const QString& code);

public slots:
    void setTestResults(const QString& resultsText, bool passed);

private:
    QString starterCode_;
    QPlainTextEdit* editor_;
    CppSyntaxHighlighter* highlighter_;
    QPushButton* runBtn_;
    QPushButton* resetBtn_;
    QLabel* outputLabel_;
};
