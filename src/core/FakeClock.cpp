#include "core/FakeClock.h"
#include <sstream>
#include <iomanip>
#include <ctime>

// OOP Concept: Test Double Implementation
// Implements manual date management and date arithmetic for unit tests.

FakeClock::FakeClock(std::string initialDate)
    : currentDate_(std::move(initialDate)) {}

std::string FakeClock::getCurrentDate() const {
    return currentDate_;
}

void FakeClock::setDate(std::string date) {
    currentDate_ = std::move(date);
}

void FakeClock::advanceDays(int days) {
    std::tm tm{};
    std::istringstream ss(currentDate_);
    char dash1, dash2;
    int year, month, day;
    if (ss >> year >> dash1 >> month >> dash2 >> day) {
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day + days;
        tm.tm_hour = 12;

        std::time_t next = std::mktime(&tm);
        std::tm* result = std::localtime(&next);
        std::stringstream out;
        out << std::put_time(result, "%Y-%m-%d");
        currentDate_ = out.str();
    }
}
