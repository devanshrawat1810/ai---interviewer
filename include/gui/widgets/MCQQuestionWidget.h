#pragma once

#include "gui/widgets/QuestionViewFactory.h"
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <vector>
#include <string>

// OOP Concept: Concrete View Specialization
// Renders MCQ options with keyboard shortcuts (1-4, A-D) and selection animations.
class MCQQuestionWidget : public BaseQuestionWidget {
    Q_OBJECT

public:
    explicit MCQQuestionWidget(const std::vector<std::string>& options, QWidget* parent = nullptr);

    QString getAnswer() const override;
    void clearAnswer() override;

private:
    QButtonGroup* group_;
    std::vector<QRadioButton*> radioButtons_;
};
