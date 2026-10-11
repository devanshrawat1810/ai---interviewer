#pragma once

#include "core/Student.h"
#include <memory>
#include <vector>

// OOP Concept: Strategy Pattern & Open/Closed Principle
// Defines the evaluation strategy interface for qualifying badges.
class BadgeRule {
public:
    virtual ~BadgeRule() = default;
    virtual bool isEligible(const Student& student) const = 0;
    virtual std::string getBadgeName() const = 0;
};

class FirstSolveBadgeRule : public BadgeRule {
public:
    bool isEligible(const Student& student) const override;
    std::string getBadgeName() const override;
};

class SevenDayStreakBadgeRule : public BadgeRule {
public:
    bool isEligible(const Student& student) const override;
    std::string getBadgeName() const override;
};

class ThirtyDayStreakBadgeRule : public BadgeRule {
public:
    bool isEligible(const Student& student) const override;
    std::string getBadgeName() const override;
};

class TenSolvedBadgeRule : public BadgeRule {
public:
    bool isEligible(const Student& student) const override;
    std::string getBadgeName() const override;
};

class FiftySolvedBadgeRule : public BadgeRule {
public:
    bool isEligible(const Student& student) const override;
    std::string getBadgeName() const override;
};

class HardMasterBadgeRule : public BadgeRule {
public:
    bool isEligible(const Student& student) const override;
    std::string getBadgeName() const override;
};

class DailyChallengeMasterBadgeRule : public BadgeRule {
public:
    bool isEligible(const Student& student) const override;
    std::string getBadgeName() const override;
};

// Evaluator running a suite of BadgeRules against a student
class BadgeEvaluator {
private:
    std::vector<std::unique_ptr<BadgeRule>> rules_;

public:
    BadgeEvaluator();
    std::vector<std::string> evaluateNewBadges(const Student& student) const;
};
