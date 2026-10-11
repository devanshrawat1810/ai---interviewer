#pragma once

#include <memory>
#include <string>
#include "auth/AuthService.h"
#include "admin/AdminService.h"
#include "interview/DailyChallengeService.h"
#include "interview/StreakService.h"
#include "interview/ReviewService.h"
#include "interview/NotificationService.h"
#include "db/IUserRepository.h"
#include "db/IQuestionRepository.h"
#include "db/IAttemptRepository.h"
#include "db/IInterviewRepository.h"
#include "db/IFeedbackRepository.h"
#include "db/IScoreboardRepository.h"
#include "core/Student.h"
#include "core/Admin.h"
#include "course/Course.h"

// OOP Concept: Dependency Injection & Separation of Concerns
// Terminal user interface encapsulating interactive menus for students and administrators.
class ConsoleUI {
private:
    AuthService& authService_;
    AdminService& adminService_;
    DailyChallengeService& dailyService_;
    StreakService& streakService_;
    ReviewService& reviewService_;
    NotificationService& noteService_;
    IUserRepository& userRepo_;
    IQuestionRepository& questionRepo_;
    IAttemptRepository& attemptRepo_;
    IInterviewRepository& interviewRepo_;
    IFeedbackRepository& feedbackRepo_;
    IScoreboardRepository& scoreboardRepo_;

    std::vector<Course> courses_;
    std::shared_ptr<User> currentUser_{nullptr};
    std::shared_ptr<Student> currentStudent_{nullptr};

    void showWelcomeMenu();
    void handleRegister();
    void handleLogin();

    // Student workflows
    void showDashboard();
    void handleTakeInterview();
    void handleDailyChallenge();
    void handleStreakCalendar();
    void handleCodingChallenges();
    void handleReviewWrongAnswers();
    void handleBookmarkedQuestions();
    void handleViewProfile();
    void handleViewLeaderboard();

    // Admin workflows
    void showAdminDashboard();
    void handleAdminStats();
    void handleAdminSearchQuestions();
    void handleAdminAddQuestion();
    void handleAdminSoftDeleteQuestion();
    void handleAdminBulkImport();
    void handleAdminViewReports();
    void handleAdminDifficultyDiscrepancies();

    const Course* selectCourse();
    const Subject* selectSubject(const Course& course);

public:
    ConsoleUI(AuthService& authService,
              AdminService& adminService,
              DailyChallengeService& dailyService,
              StreakService& streakService,
              ReviewService& reviewService,
              NotificationService& noteService,
              IUserRepository& userRepo,
              IQuestionRepository& questionRepo,
              IAttemptRepository& attemptRepo,
              IInterviewRepository& interviewRepo,
              IFeedbackRepository& feedbackRepo,
              IScoreboardRepository& scoreboardRepo);

    ~ConsoleUI() = default;

    void run();

    void setCurrentUser(std::shared_ptr<User> user);
    std::shared_ptr<User> getCurrentUser() const;
    std::shared_ptr<Student> getCurrentStudent() const;
};
