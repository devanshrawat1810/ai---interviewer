#pragma once

#include "db/IQuestionFeedbackRepository.h"
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// JSON file-backed storage for question reports and difficulty feedback.
class JsonQuestionFeedbackRepository : public IQuestionFeedbackRepository {
private:
    std::string reportsFilePath_;
    std::string feedbackFilePath_;
    mutable std::mutex mutex_;

    void ensureFilesExist() const;

public:
    explicit JsonQuestionFeedbackRepository(
        std::string reportsFilePath = "data/question_reports.json",
        std::string feedbackFilePath = "data/difficulty_feedback.json");
    ~JsonQuestionFeedbackRepository() override = default;

    void saveReport(const QuestionReport& report) override;
    std::vector<QuestionReport> getAllReports() override;

    void saveDifficultyFeedback(const DifficultyFeedback& fb) override;
    std::vector<DifficultyFeedback> getDifficultyFeedbackForQuestion(const std::string& questionId) override;
    std::vector<DifficultyFeedback> getAllDifficultyFeedback() override;
};
