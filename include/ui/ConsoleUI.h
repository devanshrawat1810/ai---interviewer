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
#include "feedback/AIFeedback.h"
#include "interview/ResumeInterviewMode.h"
#include "ai/FollowUpGenerator.h"
#include "ai/UsageTracker.h"
#include "ai/AnswerReviewer.h"

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
    void handleResumeInterview();
    void handleDailyChallenge();
    void handleStreakCalendar();
    void handleCodingChallenges();
    void handleReviewWrongAnswers();
    void handleAnswerReviewCoaching();
    void handleBookmarkedQuestions();
    void handleViewProfile();
    void handleViewLeaderboard();

    // Phase 4 AI Components
    std::shared_ptr<AIFeedback> aiFeedback_{std::make_shared<AIFeedback>()};
    std::shared_ptr<ResumeInterviewMode> resumeMode_{std::make_shared<ResumeInterviewMode>()};
    std::shared_ptr<FollowUpGenerator> followUpGen_{std::make_shared<FollowUpGenerator>()};
    std::shared_ptr<UsageTracker> usageTracker_{std::make_shared<UsageTracker>()};
    std::shared_ptr<AnswerReviewer> answerReviewer_{std::make_shared<AnswerReviewer>()};

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
