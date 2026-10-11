#include "interview/NotificationService.h"
#include <sstream>

// OOP Concept: Single Responsibility Principle
// Implementation of NotificationService.

NotificationService::NotificationService(StreakService& streakService, DailyChallengeService& dailyService)
    : streakService_(streakService),
      dailyService_(dailyService) {}

LoginNotification NotificationService::checkNotifications(
    const Student& student, const std::string& /* course */) const {
    LoginNotification note;
    note.streakCount = student.getStreak();
    note.streakAtRisk = streakService_.isStreakAtRisk(student);

    std::string today = streakService_.getCurrentDate();
    bool hasActiveToday = (student.getLastActiveDate() == today);
    note.dailyChallengeCompleted = hasActiveToday; // Flagged if completed today

    if (note.streakAtRisk) {
        note.announcementMessages.push_back(
            "ALERT: Your " + std::to_string(note.streakCount) +
            "-day streak is at risk! Complete an interview or daily challenge today to maintain it.");
    } else if (hasActiveToday) {
        note.announcementMessages.push_back(
            "Great job! You have already logged activity for today (" + today + ").");
    } else {
        note.announcementMessages.push_back(
            "Welcome! Today's Daily Challenge is ready. Earn +50 bonus points today!");
    }

    return note;
}

std::string NotificationService::renderBanner(const LoginNotification& note) const {
    std::ostringstream ss;
    ss << "\n********************** NOTIFICATIONS **********************\n";
    for (const auto& msg : note.announcementMessages) {
        ss << "  [i] " << msg << "\n";
    }
    ss << "************************************************************\n";
    return ss.str();
}
