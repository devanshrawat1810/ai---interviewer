#pragma once

#include "core/User.h"
#include <map>
#include <set>
#include <vector>
#include "core/Difficulty.h"

// OOP Concept: Inheritance
// Student specializes User by maintaining academic metrics, coding challenge completions,
// difficulty unlock tiers per subject, and gamification badges.
class Student : public User {
private:
    std::map<Difficulty, int> solvedByDifficulty_;
    std::map<std::string, Difficulty> subjectUnlockedLevel_;
    std::set<std::string> solvedChallenges_;
    std::vector<std::string> badges_;
    int totalQuestionsAttempted_{0};
    int totalQuestionsCorrect_{0};
    int totalScore_{0};
    int dailyChallengesCompleted_{0};

public:
    Student(std::string id,
            std::string name,
            std::string email,
            std::string passwordHash,
            std::string salt,
            std::string course = "CSE",
            std::string college = "Unspecified",
            std::string branch = "Unspecified",
            int streak = 0,
            int longestStreak = 0,
            std::string lastActiveDate = "");

    ~Student() override = default;

    // Academic & challenge tracking methods
    void recordChallengeSolve(const std::string& challengeId, const std::string& subject, Difficulty diff, int points);
    void recordQuestionAttempt(bool correct, int points);
    void recordDailyChallengeCompletion(int bonusPoints);

    int getSolvedCount(Difficulty diff) const;
    int getTotalSolvedCount() const;
    Difficulty getUnlockedLevel(const std::string& subject) const;
    bool isLevelUnlocked(const std::string& subject, Difficulty diff) const;

    double getAccuracy() const noexcept;
    int getTotalScore() const noexcept;
    int getDailyChallengesCompleted() const noexcept;
    const std::vector<std::string>& getBadges() const noexcept;
    const std::set<std::string>& getSolvedChallenges() const noexcept;

    void addBadge(const std::string& badgeName);
    bool hasBadge(const std::string& badgeName) const;

    // Direct setters for database hydration
    void setBadges(std::vector<std::string> badges);
    void setSubjectUnlockedLevel(std::map<std::string, Difficulty> levels);
    void setSolvedChallenges(std::set<std::string> challenges);
    void setSolvedCounts(int easy, int medium, int hard);
    void setTotalQuestionsAttempted(int count);
    void setTotalQuestionsCorrect(int count);
    void setTotalScore(int score);
    void setDailyChallengesCompleted(int count);
};
