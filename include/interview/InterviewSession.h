#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "core/Question.h"
#include "core/Difficulty.h"
#include "interview/QuestionSelectionStrategy.h"

// Record of a single question answered during an interview session
struct SessionQuestionRecord {
    std::shared_ptr<Question> question;
    std::string userResponse;
    bool isCorrect{false};
    int timeTakenSeconds{0};
    int timeLimitSeconds{0};
    int pointsEarned{0};
    int maxPoints{0};
};

// OOP Concept: Encapsulation & Orchestration
// Manages the complete lifecycle of a candidate's mock interview session.
// Enforces time limits per difficulty tier and aggregates weighted scores.
class InterviewSession {
private:
    std::string id_;
    std::string userId_;
    std::string subjectId_;
    std::string mode_; // "practice", "mock", "daily"
    int targetQuestionCount_{10};
    std::unique_ptr<QuestionSelectionStrategy> strategy_;
    std::vector<std::shared_ptr<Question>> questionPool_;
    std::vector<SessionQuestionRecord> records_;
    std::vector<bool> resultHistory_;
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point endTime_;
    bool isCompleted_{false};

public:
    InterviewSession(std::string id,
                     std::string userId,
                     std::string subjectId,
                     std::string mode,
                     std::unique_ptr<QuestionSelectionStrategy> strategy,
                     std::vector<std::shared_ptr<Question>> questionPool,
                     int targetQuestionCount = 10);

    ~InterviewSession() = default;

    void start();
    void finish();

    // Retrieves time limit in seconds based on difficulty:
    // EASY = 60s (1 min), MEDIUM = 120s (2 mins), HARD = 240s (4 mins)
    static int getTimeLimitForDifficulty(Difficulty diff);

    // Calculates weighted points for difficulty:
    // EASY = 1 pt, MEDIUM = 2 pts, HARD = 3 pts
    static int getWeightForDifficulty(Difficulty diff);

    // Advances the interview by picking the next question according to strategy
    std::shared_ptr<Question> getNextQuestion();

    // Records the user's answer and evaluates correctness and points
    bool submitAnswer(std::shared_ptr<Question> question,
                      const std::string& answer,
                      int timeTakenSeconds);

    // Summary statistics
    const std::string& getId() const noexcept;
    const std::string& getUserId() const noexcept;
    const std::string& getSubjectId() const noexcept;
    const std::string& getMode() const noexcept;
    int getTotalScore() const noexcept;
    int getMaxPossibleScore() const noexcept;
    int getCorrectCount() const noexcept;
    int getQuestionsAttempted() const noexcept;
    int getTargetQuestionCount() const noexcept;
    bool isCompleted() const noexcept;
    const std::vector<SessionQuestionRecord>& getRecords() const noexcept;
};
