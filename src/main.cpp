#include "db/RepositoryFactory.h"
#include "auth/AuthService.h"
#include "admin/AdminService.h"
#include "interview/DailyChallengeService.h"
#include "interview/StreakService.h"
#include "interview/ReviewService.h"
#include "interview/NotificationService.h"
#include "core/SystemClock.h"
#include "ui/ConsoleUI.h"
#include <iostream>

// Entry point of AI Interviewer application.
// Applies Dependency Injection via constructor wiring.

int main() {
    std::cout << "[AI Interviewer] Initializing system components (Phase 3)...\n";

    // 1. Resolve and instantiate repositories via RepositoryFactory
    auto repos = RepositoryFactory::createRepositories();
    std::cout << "[AI Interviewer] Active Persistence Backend: [" 
              << repos.activeBackend << "]\n";

    // 2. Instantiate system clock
    SystemClock clock;

    // 3. Instantiate core domain services
    AuthService authService(*repos.userRepo);
    AdminService adminService(*repos.userRepo, *repos.questionRepo, *repos.attemptRepo, *repos.questionFeedbackRepo);
    DailyChallengeService dailyService(*repos.dailyRepo, *repos.questionRepo, clock);
    StreakService streakService(*repos.userRepo, clock);
    ReviewService reviewService(*repos.attemptRepo, *repos.questionRepo, *repos.userRepo, *repos.questionFeedbackRepo);
    NotificationService noteService(streakService, dailyService);

    // 4. Instantiate ConsoleUI injecting services and repositories
    ConsoleUI ui(authService,
                 adminService,
                 dailyService,
                 streakService,
                 reviewService,
                 noteService,
                 *repos.userRepo,
                 *repos.questionRepo,
                 *repos.attemptRepo,
                 *repos.interviewRepo,
                 *repos.feedbackRepo,
                 *repos.scoreboardRepo);

    // 5. Start user interaction loop
    ui.run();

    return 0;
}
