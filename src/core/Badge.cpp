#include "core/Badge.h"

// OOP Concept: Polymorphism & Inheritance
// Implementation of extended Badge constructors.

Badge::Badge(std::string id, std::string name, std::string description)
    : id_(std::move(id)),
      name_(std::move(name)),
      description_(std::move(description)) {}

const std::string& Badge::getId() const noexcept { return id_; }
const std::string& Badge::getName() const noexcept { return name_; }
const std::string& Badge::getDescription() const noexcept { return description_; }

FirstSolveBadge::FirstSolveBadge()
    : Badge("badge_first_solve", "First Solve", "Awarded for completing your first coding challenge.") {}

SevenDayStreakBadge::SevenDayStreakBadge()
    : Badge("badge_7day_streak", "7-Day Streak", "Awarded for practicing consecutively for 7 days.") {}

ThirtyDayStreakBadge::ThirtyDayStreakBadge()
    : Badge("badge_30day_streak", "30-Day Streak", "Awarded for practicing consecutively for 30 days.") {}

TenSolvedBadge::TenSolvedBadge()
    : Badge("badge_10_solved", "10 Solved", "Awarded for successfully solving 10 coding challenges.") {}

FiftySolvedBadge::FiftySolvedBadge()
    : Badge("badge_50_solved", "50 Solved", "Awarded for successfully solving 50 coding challenges.") {}

HardMasterBadge::HardMasterBadge()
    : Badge("badge_hard_master", "Hard Master", "Awarded for solving at least one Hard challenge.") {}

PerfectInterviewBadge::PerfectInterviewBadge()
    : Badge("badge_perfect_interview", "Perfect Interview", "Awarded for achieving 100% score on a mock interview.") {}

DailyChallengeMasterBadge::DailyChallengeMasterBadge()
    : Badge("badge_daily_master", "Daily Challenge Master", "Awarded for completing at least 5 daily challenges.") {}
