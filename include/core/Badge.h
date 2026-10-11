#pragma once

#include <string>

// OOP Concept: Polymorphism & Inheritance
// Base Badge class defining metadata for student achievement milestones.
class Badge {
protected:
    std::string id_;
    std::string name_;
    std::string description_;

public:
    Badge(std::string id, std::string name, std::string description);
    virtual ~Badge() = default;

    const std::string& getId() const noexcept;
    const std::string& getName() const noexcept;
    const std::string& getDescription() const noexcept;
};

class FirstSolveBadge : public Badge {
public:
    FirstSolveBadge();
};

class SevenDayStreakBadge : public Badge {
public:
    SevenDayStreakBadge();
};

class ThirtyDayStreakBadge : public Badge {
public:
    ThirtyDayStreakBadge();
};

class TenSolvedBadge : public Badge {
public:
    TenSolvedBadge();
};

class FiftySolvedBadge : public Badge {
public:
    FiftySolvedBadge();
};

class HardMasterBadge : public Badge {
public:
    HardMasterBadge();
};

class PerfectInterviewBadge : public Badge {
public:
    PerfectInterviewBadge();
};

class DailyChallengeMasterBadge : public Badge {
public:
    DailyChallengeMasterBadge();
};
