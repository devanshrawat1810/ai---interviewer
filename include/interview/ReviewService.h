#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/Question.h"
#include "core/Student.h"
#include "db/IAttemptRepository.h"
#include "db/IQuestionRepository.h"
#include "db/IUserRepository.h"
#include "db/IQuestionFeedbackRepository.h"

struct WrongAttemptReview {
    std::string attemptId;
    std::string questionId;
    std::string questionText;
    std::string userResponse;
    std::string correctAnswer;
    std::string explanation;
    Difficulty difficulty;
    std::string date;
    std::shared_ptr<Question> question;
};

// OOP Concept: Single Responsibility Principle & Facade
// Coordinates learning review workflows, wrong attempt retries, bookmarking, and student question feedback.
class ReviewService {
private:
    IAttemptRepository& attemptRepo_;
    IQuestionRepository& questionRepo_;
    IUserRepository& userRepo_;
    IQuestionFeedbackRepository& feedbackRepo_;

public:
    ReviewService(IAttemptRepository& attemptRepo,
                  IQuestionRepository& questionRepo,
                  IUserRepository& userRepo,
                  IQuestionFeedbackRepository& feedbackRepo);
    ~ReviewService() = default;

    std::vector<WrongAttemptReview> getWrongAttempts(const std::string& userId) const;

    bool toggleBookmark(Student& student, const std::string& questionId);
    std::vector<std::shared_ptr<Question>> getBookmarkedQuestions(
        const Student& student, const std::string& subject = "") const;

    void reportQuestion(const std::string& userId,
                        const std::string& questionId,
                        const std::string& reason);

    void submitDifficultyFeedback(const std::string& userId,
                                  const std::string& questionId,
                                  const std::string& feedback);
};
