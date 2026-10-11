#pragma once

#include <string>
#include <vector>
#include "core/Student.h"
#include "interview/StreakService.h"
#include "interview/DailyChallengeService.h"

struct LoginNotification {
    bool streakAtRisk{false};
    int streakCount{0};
    bool dailyChallengeCompleted{false};
    std::vector<std::string> newlyEarnedBadges;
    std::vector<std::string> announcementMessages;
};

// OOP Concept: Single Responsibility Principle & Presentation Decoupling
// NotificationService evaluates student status on login and formulates console alert summaries.
class NotificationService {
private:
    StreakService& streakService_;
    DailyChallengeService& dailyService_;

public:
    NotificationService(StreakService& streakService, DailyChallengeService& dailyService);
    ~NotificationService() = default;

    LoginNotification checkNotifications(const Student& student, const std::string& course) const;
    std::string renderBanner(const LoginNotification& note) const;
};
