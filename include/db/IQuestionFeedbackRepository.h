#pragma once

#include <string>
#include <vector>

struct QuestionReport {
    std::string id;
    std::string questionId;
    std::string userId;
    std::string reason;
    std::string reportedAt;
};

struct DifficultyFeedback {
    std::string id;
    std::string questionId;
    std::string userId;
    std::string feedback; // "too_easy", "too_hard", "appropriate"
    std::string submittedAt;
};

// OOP Concept: Dependency Inversion Principle
// Repository interface for community question reports and difficulty perception feedback.
class IQuestionFeedbackRepository {
public:
    virtual ~IQuestionFeedbackRepository() = default;

    virtual void saveReport(const QuestionReport& report) = 0;
    virtual std::vector<QuestionReport> getAllReports() = 0;

    virtual void saveDifficultyFeedback(const DifficultyFeedback& fb) = 0;
    virtual std::vector<DifficultyFeedback> getDifficultyFeedbackForQuestion(const std::string& questionId) = 0;
    virtual std::vector<DifficultyFeedback> getAllDifficultyFeedback() = 0;
};
