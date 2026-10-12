#include "gui/widgets/QuestionViewFactory.h"
#include "gui/widgets/MCQQuestionWidget.h"
#include "gui/widgets/TextQuestionWidget.h"
#include "gui/widgets/CodingQuestionWidget.h"
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include "core/CodingQuestion.h"

BaseQuestionWidget* QuestionViewFactory::createView(const std::shared_ptr<Question>& question, QWidget* parent) {
    if (!question) {
        return new TextQuestionWidget(parent);
    }

    if (auto mcq = std::dynamic_pointer_cast<MCQQuestion>(question)) {
        return new MCQQuestionWidget(mcq->getOptions(), parent);
    }

    if (auto coding = std::dynamic_pointer_cast<CodingQuestion>(question)) {
        return new CodingQuestionWidget(QString::fromStdString(coding->getStarterCode()), parent);
    }

    return new TextQuestionWidget(parent);
}
