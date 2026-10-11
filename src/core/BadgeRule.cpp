#include "core/BadgeRule.h"

// OOP Concept: Strategy Pattern Implementation
// Concrete evaluation rules checking student milestone criteria.

bool FirstSolveBadgeRule::isEligible(const Student& student) const {
    return student.getTotalSolvedCount() >= 1;
}

std::string FirstSolveBadgeRule::getBadgeName() const {
    return "First Solve";
}

bool SevenDayStreakBadgeRule::isEligible(const Student& student) const {
    return student.getStreak() >= 7;
}

std::string SevenDayStreakBadgeRule::getBadgeName() const {
    return "7-Day Streak";
}

bool ThirtyDayStreakBadgeRule::isEligible(const Student& student) const {
    return student.getStreak() >= 30;
}

std::string ThirtyDayStreakBadgeRule::getBadgeName() const {
    return "30-Day Streak";
}

bool TenSolvedBadgeRule::isEligible(const Student& student) const {
    return student.getTotalSolvedCount() >= 10;
}

std::string TenSolvedBadgeRule::getBadgeName() const {
    return "10 Solved";
}

bool FiftySolvedBadgeRule::isEligible(const Student& student) const {
    return student.getTotalSolvedCount() >= 50;
}

std::string FiftySolvedBadgeRule::getBadgeName() const {
    return "50 Solved";
}

bool HardMasterBadgeRule::isEligible(const Student& student) const {
    return student.getSolvedCount(Difficulty::HARD) >= 1;
}

std::string HardMasterBadgeRule::getBadgeName() const {
    return "Hard Master";
}

bool DailyChallengeMasterBadgeRule::isEligible(const Student& student) const {
    return student.getDailyChallengesCompleted() >= 5;
}

std::string DailyChallengeMasterBadgeRule::getBadgeName() const {
    return "Daily Challenge Master";
}

BadgeEvaluator::BadgeEvaluator() {
    rules_.push_back(std::make_unique<FirstSolveBadgeRule>());
    rules_.push_back(std::make_unique<SevenDayStreakBadgeRule>());
    rules_.push_back(std::make_unique<ThirtyDayStreakBadgeRule>());
    rules_.push_back(std::make_unique<TenSolvedBadgeRule>());
    rules_.push_back(std::make_unique<FiftySolvedBadgeRule>());
    rules_.push_back(std::make_unique<HardMasterBadgeRule>());
    rules_.push_back(std::make_unique<DailyChallengeMasterBadgeRule>());
}

std::vector<std::string> BadgeEvaluator::evaluateNewBadges(const Student& student) const {
    std::vector<std::string> newlyAwarded;
    for (const auto& rule : rules_) {
        std::string badgeName = rule->getBadgeName();
        if (!student.hasBadge(badgeName) && rule->isEligible(student)) {
            newlyAwarded.push_back(badgeName);
        }
    }
    return newlyAwarded;
}
