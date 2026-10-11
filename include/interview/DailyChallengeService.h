#pragma once

#include <string>
#include <memory>
#include <vector>
#include "core/Question.h"
#include "core/Student.h"
#include "core/IClock.h"
#include "db/IDailyChallengeRepository.h"
#include "db/IQuestionRepository.h"

// OOP Concept: Single Responsibility Principle & Strategy Orchestration
// DailyChallengeService deterministically provisions daily placement questions,
// enforces 30-day non-repetition, and manages bonus awards.
class DailyChallengeService {
private:
    IDailyChallengeRepository& dailyRepo_;
    IQuestionRepository& questionRepo_;
    const IClock& clock_;

public:
    DailyChallengeService(IDailyChallengeRepository& dailyRepo,
                          IQuestionRepository& questionRepo,
                          const IClock& clock);
    ~DailyChallengeService() = default;

    // Retrieves or deterministically selects today's challenge question for a course
    std::shared_ptr<Question> getDailyQuestion(const std::string& course);

    // Records completion, awarding bonus points (default 50 pts)
    bool completeDailyChallenge(Student& student, const std::string& course, bool isCorrect, int bonusPoints = 50);

    std::string getCurrentDate() const;
};
