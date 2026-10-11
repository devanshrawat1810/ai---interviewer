#pragma once

#include "db/IQuestionFeedbackRepository.h"

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB storage implementation for question reports and difficulty feedback.
class MongoQuestionFeedbackRepository : public IQuestionFeedbackRepository {
public:
    MongoQuestionFeedbackRepository() = default;
    ~MongoQuestionFeedbackRepository() override = default;

    void saveReport(const QuestionReport& report) override;
    std::vector<QuestionReport> getAllReports() override;

    void saveDifficultyFeedback(const DifficultyFeedback& fb) override;
    std::vector<DifficultyFeedback> getDifficultyFeedbackForQuestion(const std::string& questionId) override;
    std::vector<DifficultyFeedback> getAllDifficultyFeedback() override;
};
