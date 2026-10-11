#include "interview/DailyChallengeService.h"
#include <algorithm>
#include <functional>

// OOP Concept: Single Responsibility & Deterministic Strategy
// Implementation of DailyChallengeService.

DailyChallengeService::DailyChallengeService(IDailyChallengeRepository& dailyRepo,
                                             IQuestionRepository& questionRepo,
                                             const IClock& clock)
    : dailyRepo_(dailyRepo),
      questionRepo_(questionRepo),
      clock_(clock) {}

std::string DailyChallengeService::getCurrentDate() const {
    return clock_.getCurrentDate();
}

std::shared_ptr<Question> DailyChallengeService::getDailyQuestion(const std::string& course) {
    std::string today = clock_.getCurrentDate();

    // Check if already selected for today
    auto existing = dailyRepo_.findByDateAndCourse(today, course);
    if (existing) {
        auto q = questionRepo_.findById(existing->questionId);
        if (q && q->isActive()) return q;
    }

    // Retrieve pool of questions
    auto allQuestions = questionRepo_.getAll();
    std::vector<std::shared_ptr<Question>> pool;
    for (const auto& q : allQuestions) {
        if (q->isActive()) {
            pool.push_back(q);
        }
    }

    if (pool.empty()) return nullptr;

    // Filter out questions used within past 30 days for this course
    auto recentUsedIds = dailyRepo_.getPast30DaysQuestionIds(today, course);
    std::vector<std::shared_ptr<Question>> available;
    for (const auto& q : pool) {
        if (std::find(recentUsedIds.begin(), recentUsedIds.end(), q->getId()) == recentUsedIds.end()) {
            available.push_back(q);
        }
    }

    // If all questions have been used within 30 days, fallback to entire pool
    if (available.empty()) {
        available = pool;
    }

    // Deterministic selection based on seed string: date + course
    std::string seedStr = today + "_" + course;
    std::size_t hashVal = std::hash<std::string>{}(seedStr);
    std::size_t selectedIndex = hashVal % available.size();
    auto selectedQuestion = available[selectedIndex];

    // Persist daily challenge assignment
    DailyChallengeRecord record;
    record.id = "dc_" + today + "_" + course;
    record.date = today;
    record.course = course;
    record.questionId = selectedQuestion->getId();
    dailyRepo_.save(record);

    return selectedQuestion;
}

bool DailyChallengeService::completeDailyChallenge(
    Student& student, const std::string& /* course */, bool isCorrect, int bonusPoints) {
    if (isCorrect) {
        student.recordDailyChallengeCompletion(bonusPoints);
        return true;
    }
    return false;
}
