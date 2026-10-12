#pragma once

#include "gui/widgets/QuestionViewFactory.h"
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>

// OOP Concept: Concrete View Specialization
// Multi-line editor with live character and word count tracking.
class TextQuestionWidget : public BaseQuestionWidget {
    Q_OBJECT

public:
    explicit TextQuestionWidget(QWidget* parent = nullptr);

    QString getAnswer() const override;
    void clearAnswer() override;

private:
    QTextEdit* editor_;
    QLabel* counterLabel_;
    void updateCounts();
};
