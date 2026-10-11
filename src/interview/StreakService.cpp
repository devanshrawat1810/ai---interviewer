#include "interview/StreakService.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

// OOP Concept: Single Responsibility & Injected Clock
// Implementation of StreakService.

StreakService::StreakService(IUserRepository& userRepo, const IClock& clock)
    : userRepo_(userRepo),
      clock_(clock) {}

std::string StreakService::getCurrentDate() const {
    return clock_.getCurrentDate();
}

int StreakService::daysDifference(const std::string& date1, const std::string& date2) {
    if (date1.empty() || date2.empty()) return 999;
    if (date1 == date2) return 0;

    std::tm tm1{}, tm2{};
    char dash;
    int y1, m1, d1, y2, m2, d2;

    std::istringstream s1(date1);
    std::istringstream s2(date2);
    if (!(s1 >> y1 >> dash >> m1 >> dash >> d1) || !(s2 >> y2 >> dash >> m2 >> dash >> d2)) {
        return 999;
    }

    tm1.tm_year = y1 - 1900;
    tm1.tm_mon = m1 - 1;
    tm1.tm_mday = d1;
    tm1.tm_hour = 12;

    tm2.tm_year = y2 - 1900;
    tm2.tm_mon = m2 - 1;
    tm2.tm_mday = d2;
    tm2.tm_hour = 12;

    std::time_t t1 = std::mktime(&tm1);
    std::time_t t2 = std::mktime(&tm2);

    double seconds = std::difftime(t2, t1);
    return static_cast<int>(seconds / (60.0 * 60.0 * 24.0) + 0.5);
}

void StreakService::recordActivity(User& user) {
    std::string today = clock_.getCurrentDate();

    if (user.getLastActiveDate() == today) {
        // Already active today; streak maintained
        return;
    }

    if (user.getLastActiveDate().empty()) {
        user.setStreak(1);
    } else {
        int diff = daysDifference(user.getLastActiveDate(), today);
        if (diff == 1) {
            // Consecutive day!
            user.incrementStreak();
        } else if (diff > 1) {
            // Missed a full day or more -> reset streak
            user.setStreak(1);
        } else {
            // Same day or clock skew -> maintain
            if (user.getStreak() == 0) user.setStreak(1);
        }
    }

    user.setLastActiveDate(today);
    user.addActivityDate(today);
    userRepo_.save(user);
}

bool StreakService::isStreakAtRisk(const User& user) const {
    if (user.getStreak() == 0) return false;
    std::string today = clock_.getCurrentDate();
    if (user.getLastActiveDate() == today) return false;

    int diff = daysDifference(user.getLastActiveDate(), today);
    return (diff == 1); // Yesterday active, today pending
}

std::string StreakService::renderStreakCalendar(const User& user, int year, int month) const {
    if (year == 0 || month == 0) {
        std::string today = clock_.getCurrentDate();
        std::istringstream ss(today);
        char dash;
        ss >> year >> dash >> month;
    }

    // Determine first day of month weekday and total days in month
    std::tm tm{};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = 1;
    tm.tm_hour = 12;
    std::mktime(&tm);

    int startWeekday = tm.tm_wday; // 0 = Sunday, 1 = Monday, ...

    // Days in month
    static const int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int totalDays = daysPerMonth[month - 1];
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        totalDays = 29;
    }

    const auto& activeDates = user.getActivityDates();

    std::ostringstream out;
    out << "\n================ STREAK CALENDAR ================\n";
    out << " Candidate: " << user.getName() << "\n";
    out << " Current Streak: " << user.getStreak() << " days | "
        << " Longest Streak: " << user.getLongestStreak() << " days\n";
    out << " Month: " << year << "-" << std::setfill('0') << std::setw(2) << month << "\n";
    out << "-------------------------------------------------\n";
    out << " Sun  Mon  Tue  Wed  Thu  Fri  Sat\n";

    // Pad first week
    for (int i = 0; i < startWeekday; ++i) {
        out << "     ";
    }

    int currentWeekday = startWeekday;
    for (int day = 1; day <= totalDays; ++day) {
        std::ostringstream dayStr;
        dayStr << year << "-" << std::setfill('0') << std::setw(2) << month << "-"
               << std::setfill('0') << std::setw(2) << day;

        bool active = (std::find(activeDates.begin(), activeDates.end(), dayStr.str()) != activeDates.end());

        if (active) {
            out << "[" << std::setw(2) << day << "]";
        } else {
            out << " " << std::setw(2) << day << " ";
        }
        out << " ";

        currentWeekday = (currentWeekday + 1) % 7;
        if (currentWeekday == 0 && day != totalDays) {
            out << "\n";
        }
    }

    out << "\n-------------------------------------------------\n";
    out << " Legend: [##] = Completed Activity\n";
    out << "=================================================\n";
    return out.str();
}
