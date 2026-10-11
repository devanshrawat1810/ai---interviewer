#include "interview/ReviewService.h"
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include <chrono>

// OOP Concept: Single Responsibility Principle
// Implementation of ReviewService.

ReviewService::ReviewService(IAttemptRepository& attemptRepo,
                             IQuestionRepository& questionRepo,
                             IUserRepository& userRepo,
                             IQuestionFeedbackRepository& feedbackRepo)
    : attemptRepo_(attemptRepo),
      questionRepo_(questionRepo),
      userRepo_(userRepo),
      feedbackRepo_(feedbackRepo) {}

std::vector<WrongAttemptReview> ReviewService::getWrongAttempts(const std::string& userId) const {
    std::vector<WrongAttemptReview> reviews;
    auto attempts = attemptRepo_.findByUserId(userId);

    for (const auto& att : attempts) {
        if (!att.isCorrect) {
            auto q = questionRepo_.findById(att.questionId);
            if (!q) continue;

            WrongAttemptReview rev;
            rev.attemptId = att.id;
            rev.questionId = att.questionId;
            rev.questionText = q->getText();
            rev.userResponse = att.userResponse;
            rev.explanation = q->getExplanation();
            rev.difficulty = q->getDifficulty();
            rev.date = att.date;
            rev.question = q;

            if (const auto* mcq = dynamic_cast<const MCQQuestion*>(q.get())) {
                rev.correctAnswer = mcq->getCorrectAnswer();
            } else if (const auto* txt = dynamic_cast<const TextQuestion*>(q.get())) {
                rev.correctAnswer = txt->getSampleAnswer();
            }

            reviews.push_back(rev);
        }
    }
    return reviews;
}

bool ReviewService::toggleBookmark(Student& student, const std::string& questionId) {
    bool nowBookmarked = false;
    if (student.hasBookmark(questionId)) {
        student.removeBookmark(questionId);
        nowBookmarked = false;
    } else {
        student.addBookmark(questionId);
        nowBookmarked = true;
    }
    userRepo_.save(student);
    return nowBookmarked;
}

std::vector<std::shared_ptr<Question>> ReviewService::getBookmarkedQuestions(
    const Student& student, const std::string& subject) const {
    std::vector<std::shared_ptr<Question>> results;
    for (const auto& qId : student.getBookmarks()) {
        auto q = questionRepo_.findById(qId);
        if (q && q->isActive()) {
            if (subject.empty() || q->getSubject() == subject) {
                results.push_back(q);
            }
        }
    }
    return results;
}

void ReviewService::reportQuestion(const std::string& userId,
                                   const std::string& questionId,
                                   const std::string& reason) {
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    QuestionReport report;
    report.id = "rep_" + std::to_string(now);
    report.userId = userId;
    report.questionId = questionId;
    report.reason = reason;
    report.reportedAt = "2026-10-11";
    feedbackRepo_.saveReport(report);
}

void ReviewService::submitDifficultyFeedback(const std::string& userId,
                                             const std::string& questionId,
                                             const std::string& feedback) {
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    DifficultyFeedback fb;
    fb.id = "df_" + std::to_string(now);
    fb.userId = userId;
    fb.questionId = questionId;
    fb.feedback = feedback;
    fb.submittedAt = "2026-10-11";
    feedbackRepo_.saveDifficultyFeedback(fb);
}
