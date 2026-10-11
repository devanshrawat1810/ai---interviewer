#pragma once

#include <string>
#include <vector>
#include "core/User.h"
#include "core/IClock.h"
#include "db/IUserRepository.h"

// OOP Concept: Single Responsibility Principle & Strategy Injection
// StreakService manages streak progression, resets on missed days,
// and renders monthly ASCII consistency calendars.
class StreakService {
private:
    IUserRepository& userRepo_;
    const IClock& clock_;

    static int daysDifference(const std::string& date1, const std::string& date2);

public:
    StreakService(IUserRepository& userRepo, const IClock& clock);
    ~StreakService() = default;

    // Evaluates activity for today and updates current and longest streak
    void recordActivity(User& user);

    // Checks whether candidate's streak is at risk of resetting today
    bool isStreakAtRisk(const User& user) const;

    // Renders the current month as an ASCII activity grid
    std::string renderStreakCalendar(const User& user, int year = 0, int month = 0) const;

    std::string getCurrentDate() const;
};
