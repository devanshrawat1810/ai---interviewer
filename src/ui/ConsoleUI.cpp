#include "ui/ConsoleUI.h"
#include "ui/Leaderboard.h"
#include "interview/SessionFactory.h"
#include "feedback/RuleBasedFeedback.h"
#include "feedback/AIFeedback.h"
#include "core/BadgeRule.h"
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include "core/CodingQuestion.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <sstream>

// OOP Concept: Dependency Injection & Separation of Concerns
// Interactive console UI implementation with student and administrative workflows.

ConsoleUI::ConsoleUI(AuthService& authService,
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
                     IScoreboardRepository& scoreboardRepo)
    : authService_(authService),
      adminService_(adminService),
      dailyService_(dailyService),
      streakService_(streakService),
      reviewService_(reviewService),
      noteService_(noteService),
      userRepo_(userRepo),
      questionRepo_(questionRepo),
      attemptRepo_(attemptRepo),
      interviewRepo_(interviewRepo),
      feedbackRepo_(feedbackRepo),
      scoreboardRepo_(scoreboardRepo),
      courses_(Course::getPreloadedCourses()) {}

void ConsoleUI::setCurrentUser(std::shared_ptr<User> user) {
    currentUser_ = std::move(user);
    currentStudent_ = std::dynamic_pointer_cast<Student>(currentUser_);
}

std::shared_ptr<User> ConsoleUI::getCurrentUser() const {
    return currentUser_;
}

std::shared_ptr<Student> ConsoleUI::getCurrentStudent() const {
    return currentStudent_;
}

void ConsoleUI::run() {
    std::cout << "\n=======================================================\n";
    std::cout << "         WELCOME TO THE AI INTERVIEWER SYSTEM         \n";
    std::cout << "  C++17 Adaptive Mock Interviews & Skill Development  \n";
    std::cout << "=======================================================\n";

    while (true) {
        if (!currentUser_) {
            showWelcomeMenu();
        } else if (currentUser_->isAdmin()) {
            showAdminDashboard();
        } else {
            showDashboard();
        }
    }
}

void ConsoleUI::showWelcomeMenu() {
    std::cout << "\n[1] Register New Student\n";
    std::cout << "[2] User Login (Student or Admin)\n";
    std::cout << "[3] Exit\n";
    std::cout << "Select option: ";

    int choice = 0;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    if (choice == 1) {
        handleRegister();
    } else if (choice == 2) {
        handleLogin();
    } else if (choice == 3) {
        std::cout << "Exiting application. Goodbye!\n";
        std::exit(0);
    } else {
        std::cout << "Invalid selection. Please choose 1, 2, or 3.\n";
    }
}

void ConsoleUI::handleRegister() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string name, email, password, course, college, branch;

    std::cout << "\n--- Student Registration ---\n";
    std::cout << "Full Name: ";
    std::getline(std::cin, name);
    std::cout << "Email: ";
    std::getline(std::cin, email);
    std::cout << "Password: ";
    std::getline(std::cin, password);

    std::cout << "College Name [default: College of Engineering]: ";
    std::getline(std::cin, college);
    if (college.empty()) college = "College of Engineering";

    std::cout << "Branch / Department [default: CSE]: ";
    std::getline(std::cin, branch);
    if (branch.empty()) branch = "CSE";

    std::cout << "Academic Track (CSE, ECE, Commerce, Common) [default: CSE]: ";
    std::getline(std::cin, course);
    if (course.empty()) course = "CSE";

    try {
        currentStudent_ = authService_.registerStudent(name, email, password, course, college, branch);
        currentUser_ = currentStudent_;
        std::cout << "\nRegistration successful! Welcome, " << currentStudent_->getName()
                  << " (" << currentStudent_->getCollege() << " - " << currentStudent_->getBranch() << ")!\n";
    } catch (const std::exception& e) {
        std::cout << "\nRegistration Error: " << e.what() << "\n";
    }
}

void ConsoleUI::handleLogin() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string email, password;

    std::cout << "\n--- User Login ---\n";
    std::cout << "Email: ";
    std::getline(std::cin, email);
    std::cout << "Password: ";
    std::getline(std::cin, password);

    try {
        currentUser_ = authService_.login(email, password);
        currentStudent_ = std::dynamic_pointer_cast<Student>(currentUser_);

        if (currentUser_->isAdmin()) {
            std::cout << "\nLogged in as Administrator: " << currentUser_->getName() << "\n";
        } else if (currentStudent_) {
            std::cout << "\nLogin successful! Welcome back, " << currentStudent_->getName() << "!\n";
            // Login notification banner
            auto note = noteService_.checkNotifications(*currentStudent_, currentStudent_->getCourse());
            std::cout << noteService_.renderBanner(note);
        }
    } catch (const std::exception& e) {
        std::cout << "\nLogin Failed: " << e.what() << "\n";
    }
}

void ConsoleUI::showDashboard() {
    bool aiEnabled = aiFeedback_->isConfigured();
    std::string aiStatus = aiEnabled ? "Enabled (" + aiFeedback_->getProviderName() + ")" : "Disabled (set AI_API_KEY to enable)";
    int aiRemaining = aiEnabled ? usageTracker_->getRemainingCalls(currentStudent_->getId()) : 0;

    std::cout << "\n================ STUDENT DASHBOARD ================\n";
    const char* demoEnv = std::getenv("DEMO_MODE");
    if (demoEnv && (std::string(demoEnv) == "1" || std::string(demoEnv) == "true")) {
        std::cout << " [*** DEMO MODE ACTIVE - Offline LLM Emulation ***]\n";
    }
    std::cout << " Candidate: " << currentStudent_->getName()
              << " | " << currentStudent_->getCollege() << " (" << currentStudent_->getBranch() << ")\n"
              << " Track: " << currentStudent_->getCourse()
              << " | Streak: " << currentStudent_->getStreak() << "d (Best: " << currentStudent_->getLongestStreak() << "d)"
              << " | Score: " << currentStudent_->getTotalScore() << " pts\n";
    if (aiEnabled) {
        std::cout << " AI Features: ON | Daily AI Quota Remaining: " << aiRemaining << "\n";
    } else {
        std::cout << " AI Features: " << aiStatus << "\n";
    }
    std::cout << "---------------------------------------------------\n";
    std::cout << "[1] Start Mock Interview (10-question ramp)\n";
    std::cout << "[2] Start Practice Session (5-question adaptive)\n";
    std::cout << "[3] Resume-Based Interview" << (aiEnabled ? "" : " [AI Disabled]") << "\n";
    std::cout << "[4] Daily Challenge (+50 bonus points)\n";
    std::cout << "[5] View Streak Calendar (Monthly Grid)\n";
    std::cout << "[6] Coding Challenges Hub\n";
    std::cout << "[7] Review Wrong Answers & Retry\n";
    std::cout << "[8] Answer Review Coaching (AI)" << (aiEnabled ? "" : " [AI Disabled]") << "\n";
    std::cout << "[9] Bookmarked Questions\n";
    std::cout << "[10] Filtered Leaderboard & Subject Progress\n";
    std::cout << "[11] View & Edit Profile / Badges\n";
    std::cout << "[12] Logout\n";
    std::cout << "Choose an action: ";

    int choice = 0;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    switch (choice) {
        case 1:
        case 2:
            handleTakeInterview();
            break;
        case 3:
            handleResumeInterview();
            break;
        case 4:
            handleDailyChallenge();
            break;
        case 5:
            handleStreakCalendar();
            break;
        case 6:
            handleCodingChallenges();
            break;
        case 7:
            handleReviewWrongAnswers();
            break;
        case 8:
            handleAnswerReviewCoaching();
            break;
        case 9:
            handleBookmarkedQuestions();
            break;
        case 10:
            handleViewLeaderboard();
            break;
        case 11:
            handleViewProfile();
            break;
        case 12:
            std::cout << "Logging out...\n";
            currentUser_ = nullptr;
            currentStudent_ = nullptr;
            break;
        default:
            std::cout << "Invalid option. Please try again.\n";
            break;
    }
}


void ConsoleUI::handleDailyChallenge() {
    std::string course = currentStudent_->getCourse();
    auto question = dailyService_.getDailyQuestion(course);

    if (!question) {
        std::cout << "\nNo daily challenge available for course '" << course << "' today.\n";
        return;
    }

    std::cout << "\n================ TODAY'S DAILY CHALLENGE ================\n";
    std::cout << "Date: " << dailyService_.getCurrentDate() << " | Course: " << course << "\n";
    std::cout << "Bonus: +50 Points | Difficulty: " << difficultyToString(question->getDifficulty()) << "\n";
    std::cout << "---------------------------------------------------------\n";
    question->display();

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "\nYour answer: ";
    std::string answer;
    std::getline(std::cin, answer);

    bool correct = question->evaluate(answer);
    if (correct) {
        std::cout << "\n[+] CORRECT! You earned 50 bonus points and advanced your streak!\n";
        dailyService_.completeDailyChallenge(*currentStudent_, course, true, 50);
        streakService_.recordActivity(*currentStudent_);
    } else {
        std::cout << "\n[-] Incorrect. " << question->getExplanation() << "\n";
        streakService_.recordActivity(*currentStudent_);
    }

    // Evaluate badges
    BadgeEvaluator evaluator;
    auto newBadges = evaluator.evaluateNewBadges(*currentStudent_);
    for (const auto& b : newBadges) {
        currentStudent_->addBadge(b);
        std::cout << ">>> NEW BADGE UNLOCKED: [" << b << "] <<<\n";
    }

    userRepo_.save(*currentStudent_);
}

void ConsoleUI::handleStreakCalendar() {
    std::cout << streakService_.renderStreakCalendar(*currentStudent_);
}

const Course* ConsoleUI::selectCourse() {
    std::cout << "\nSelect Curriculum Course:\n";
    for (size_t i = 0; i < courses_.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << courses_[i].getName() << " (" << courses_[i].getId() << ")\n";
    }
    std::cout << "Select (1-" << courses_.size() << "): ";
    size_t idx = 1;
    if (!(std::cin >> idx) || idx < 1 || idx > courses_.size()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return &courses_[0];
    }
    return &courses_[idx - 1];
}

const Subject* ConsoleUI::selectSubject(const Course& course) {
    const auto& subjects = course.getSubjects();
    if (subjects.empty()) return nullptr;

    std::cout << "\nSelect Subject Track under " << course.getName() << ":\n";
    for (size_t i = 0; i < subjects.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << subjects[i].getName() << " (" << subjects[i].getId() << ")\n";
    }
    std::cout << "Select (1-" << subjects.size() << "): ";
    size_t idx = 1;
    if (!(std::cin >> idx) || idx < 1 || idx > subjects.size()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return &subjects[0];
    }
    return &subjects[idx - 1];
}

void ConsoleUI::handleTakeInterview() {
    const Course* course = selectCourse();
    if (!course) return;

    const Subject* subject = selectSubject(*course);
    if (!subject) return;

    std::string subjectId = subject->getId();

    auto questions = questionRepo_.findBySubject(subjectId);
    if (questions.empty()) {
        questions = questionRepo_.getAll();
    }

    // Filter active questions
    std::vector<std::shared_ptr<Question>> activeQuestions;
    for (const auto& q : questions) {
        if (q->isActive()) activeQuestions.push_back(q);
    }

    if (activeQuestions.empty()) {
        std::cout << "\nNo active questions available for this subject yet.\n";
        return;
    }

    auto session = SessionFactory::createSession(
        SessionType::MOCK,
        currentStudent_->getId(),
        subjectId,
        activeQuestions
    );

    session->start();
    std::cout << "\n=======================================================\n";
    std::cout << " Starting Mock Interview on Subject: " << subject->getName() << "\n";
    std::cout << " Structure: Questions 1-3 Easy | 4-7 Medium | 8-10 Hard\n";
    std::cout << " Actions available per question: type 'bookmark' to save, 'report <reason>' to report.\n";
    std::cout << "=======================================================\n";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (!session->isCompleted()) {
        auto q = session->getNextQuestion();
        if (!q) break;

        int qNum = session->getQuestionsAttempted() + 1;
        std::cout << "\n--- Question #" << qNum << " of " << session->getTargetQuestionCount() << " ---\n";
        q->display();

        int limit = InterviewSession::getTimeLimitForDifficulty(q->getDifficulty());
        std::cout << "\n[Time Limit: " << limit << "s | Points: "
                  << InterviewSession::getWeightForDifficulty(q->getDifficulty()) << "]\n";
        std::cout << "Your answer: ";

        auto start = std::chrono::steady_clock::now();
        std::string response;
        std::getline(std::cin, response);
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start).count();

        // Check for inline action commands
        if (response.find("bookmark") == 0) {
            reviewService_.toggleBookmark(*currentStudent_, q->getId());
            std::cout << "[*] Question bookmarked! Please enter your answer: ";
            std::getline(std::cin, response);
        } else if (response.find("report ") == 0) {
            std::string reason = response.substr(7);
            reviewService_.reportQuestion(currentStudent_->getId(), q->getId(), reason);
            std::cout << "[*] Report submitted to admin. Please enter your answer: ";
            std::getline(std::cin, response);
        }

        int timeTaken = static_cast<int>(elapsed);
        bool correct = session->submitAnswer(q, response, timeTaken);

        if (timeTaken > limit) {
            std::cout << "[-] Time Limit Exceeded (" << timeTaken << "s > " << limit << "s)! Marked Incorrect.\n";
        } else if (correct) {
            std::cout << "[+] Correct! Earned " << InterviewSession::getWeightForDifficulty(q->getDifficulty()) << " points.\n";
        } else {
            std::cout << "[-] Incorrect. " << q->getExplanation() << "\n";
        }

        QuestionAttempt att;
        att.id = "att_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        att.userId = currentStudent_->getId();
        att.questionId = q->getId();
        att.subject = q->getSubject();
        att.difficulty = q->getDifficulty();
        att.userResponse = response;
        att.isCorrect = correct;
        att.timeTakenSeconds = timeTaken;
        att.pointsEarned = correct ? InterviewSession::getWeightForDifficulty(q->getDifficulty()) : 0;
        att.date = streakService_.getCurrentDate();
        attemptRepo_.save(att);

        currentStudent_->recordQuestionAttempt(correct, att.pointsEarned);
    }

    session->finish();
    streakService_.recordActivity(*currentStudent_);

    FeedbackReport report = aiFeedback_->generateFeedback(*session);

    std::cout << "\n=======================================================\n";
    std::cout << "             INTERVIEW FEEDBACK REPORT\n";
    if (report.isAiGenerated) {
        std::cout << " ** " << report.notice << " **\n";
    } else if (!report.notice.empty()) {
        std::cout << " " << report.notice << "\n";
    }
    std::cout << "=======================================================\n";
    std::cout << "Final Score: " << report.totalScore << " / " << report.maxScore
              << " (" << std::fixed << std::setprecision(1) << report.percentage << "%)\n";
    std::cout << "Score by difficulty: Easy " << report.scoreByDifficulty[Difficulty::EASY] << "/" << report.maxByDifficulty[Difficulty::EASY]
              << " | Med " << report.scoreByDifficulty[Difficulty::MEDIUM] << "/" << report.maxByDifficulty[Difficulty::MEDIUM]
              << " | Hard " << report.scoreByDifficulty[Difficulty::HARD] << "/" << report.maxByDifficulty[Difficulty::HARD] << "\n";
    std::cout << "Strongest Topic: " << report.strongestTopic << "\n";
    std::cout << "Weakest Topic:   " << report.weakestTopic << "\n";

    if (report.isAiGenerated) {
        std::cout << "\nOverall Summary: " << report.overallSummary << "\n";
        std::cout << "Communication Clarity: " << report.communicationClarity << "/5 | "
                  << "Answer Structure: " << report.answerStructure << "/5 | "
                  << "Confidence Cues: " << report.confidenceCues << "/5\n";
        if (!report.strengths.empty()) {
            std::cout << "\nStrengths:\n";
            for (const auto& s : report.strengths) std::cout << "  [+] " << s << "\n";
        }
        if (!report.weaknesses.empty()) {
            std::cout << "\nAreas to Improve:\n";
            for (const auto& w : report.weaknesses) std::cout << "  [-] " << w << "\n";
        }
    }

    std::cout << "\nRecommendations:\n";
    for (const auto& sug : report.suggestions) {
        std::cout << "  * " << sug << "\n";
    }
    std::cout << "=======================================================\n";

    StoredInterviewSession storedSess;
    storedSess.id = session->getId();
    storedSess.userId = currentStudent_->getId();
    storedSess.subjectId = session->getSubjectId();
    storedSess.mode = session->getMode();
    storedSess.totalScore = session->getTotalScore();
    storedSess.maxScore = session->getMaxPossibleScore();
    storedSess.questionsAttempted = session->getQuestionsAttempted();
    storedSess.correctCount = session->getCorrectCount();
    storedSess.date = streakService_.getCurrentDate();
    interviewRepo_.save(storedSess);
    feedbackRepo_.save(report);

    // Evaluate badges immediately after interview
    BadgeEvaluator evaluator;
    auto newBadges = evaluator.evaluateNewBadges(*currentStudent_);
    for (const auto& b : newBadges) {
        currentStudent_->addBadge(b);
        std::cout << ">>> NEW BADGE EARNED: [" << b << "] <<<\n";
    }

    userRepo_.save(*currentStudent_);
}

void ConsoleUI::handleCodingChallenges() {
    std::cout << "\n--- Coding Challenges Hub ---\n";
    std::string subject = "cpp_oop";
    Difficulty unlocked = currentStudent_->getUnlockedLevel(subject);

    std::cout << "Current Subject: " << subject
              << " | Unlocked Tier: " << difficultyToString(unlocked) << "\n";

    auto allQuestions = questionRepo_.findBySubject(subject);
    std::vector<std::shared_ptr<CodingQuestion>> codingQuestions;
    for (const auto& q : allQuestions) {
        if (!q->isActive()) continue;
        auto cq = std::dynamic_pointer_cast<CodingQuestion>(q);
        if (cq) codingQuestions.push_back(cq);
    }

    if (codingQuestions.empty()) {
        std::cout << "\nNo coding challenges found in database.\n";
        return;
    }

    for (size_t i = 0; i < codingQuestions.size(); ++i) {
        auto& cq = codingQuestions[i];
        bool isUnlocked = currentStudent_->isLevelUnlocked(subject, cq->getDifficulty());
        bool isSolved = currentStudent_->getSolvedChallenges().count(cq->getId()) > 0;

        std::cout << "  [" << (i + 1) << "] "
                  << "[" << difficultyToString(cq->getDifficulty()) << "] "
                  << cq->getText()
                  << (isSolved ? " [SOLVED]" : "")
                  << (!isUnlocked ? " [LOCKED]" : "") << "\n";
    }

    std::cout << "Select challenge to solve (0 to cancel): ";
    size_t choice = 0;
    if (!(std::cin >> choice) || choice == 0 || choice > codingQuestions.size()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    auto selected = codingQuestions[choice - 1];
    if (!currentStudent_->isLevelUnlocked(subject, selected->getDifficulty())) {
        std::cout << "\n[!] This tier is LOCKED. Solve more prerequisite challenges to unlock!\n";
        return;
    }

    selected->display();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Paste your C++17 solution code below. Enter 'END' when finished:\n";

    std::string code;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "END") break;
        code += line + "\n";
    }

    std::cout << "\n[Compiler] Running solution with 2-second timeout...\n";
    auto result = selected->runSolution(code, 2);

    if (!result.compiled) {
        std::cout << "[-] Compilation Failed!\n" << result.compilerOutput << "\n";
        return;
    }

    std::cout << "[+] Tests: " << result.passedCases << "/" << result.totalCases << " passed.\n";
    if (result.passedCases == result.totalCases && result.totalCases > 0) {
        std::cout << "[*] CHALLENGE COMPLETED! +" << selected->getPoints() << " points!\n";
        currentStudent_->recordChallengeSolve(selected->getId(), subject, selected->getDifficulty(), selected->getPoints());
        streakService_.recordActivity(*currentStudent_);

        BadgeEvaluator evaluator;
        auto newBadges = evaluator.evaluateNewBadges(*currentStudent_);
        for (const auto& b : newBadges) {
            currentStudent_->addBadge(b);
            std::cout << ">>> NEW BADGE UNLOCKED: [" << b << "] <<<\n";
        }

        userRepo_.save(*currentStudent_);
    }
}

void ConsoleUI::handleReviewWrongAnswers() {
    auto reviews = reviewService_.getWrongAttempts(currentStudent_->getId());
    if (reviews.empty()) {
        std::cout << "\nCongratulations! You have zero recorded wrong answers!\n";
        return;
    }

    std::cout << "\n================ WRONG-ANSWER REVIEW ================\n";
    for (size_t i = 0; i < reviews.size(); ++i) {
        const auto& r = reviews[i];
        std::cout << "  [" << (i + 1) << "] [" << difficultyToString(r.difficulty) << "] "
                  << r.questionText << "\n"
                  << "      Your Answer:     " << r.userResponse << "\n"
                  << "      Correct Answer:  " << r.correctAnswer << "\n"
                  << "      Explanation:     " << r.explanation << "\n";
    }
    std::cout << "-----------------------------------------------------\n";
    std::cout << "Select question # to retry (0 to return): ";
    size_t retryIdx = 0;
    if (!(std::cin >> retryIdx) || retryIdx == 0 || retryIdx > reviews.size()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    const auto& targetRev = reviews[retryIdx - 1];
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "\nRetrying Question:\n";
    targetRev.question->display();
    std::cout << "Your new answer: ";
    std::string retryAnswer;
    std::getline(std::cin, retryAnswer);

    if (targetRev.question->evaluate(retryAnswer)) {
        std::cout << "[+] Correct! Excellent revision!\n";
    } else {
        std::cout << "[-] Still incorrect. Correct answer was: " << targetRev.correctAnswer << "\n";
    }
}

void ConsoleUI::handleBookmarkedQuestions() {
    auto bookmarked = reviewService_.getBookmarkedQuestions(*currentStudent_);
    if (bookmarked.empty()) {
        std::cout << "\nYou have no bookmarked questions. Use 'bookmark' during interviews to save questions.\n";
        return;
    }

    std::cout << "\n================ BOOKMARKED QUESTIONS ================\n";
    for (size_t i = 0; i < bookmarked.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] [" << bookmarked[i]->getSubject() << "] "
                  << "[" << difficultyToString(bookmarked[i]->getDifficulty()) << "] "
                  << bookmarked[i]->getText() << "\n";
    }
}

void ConsoleUI::handleViewProfile() {
    std::cout << "\n================ CANDIDATE PROFILE ================\n";
    std::cout << "Name:           " << currentStudent_->getName() << "\n";
    std::cout << "Email:          " << currentStudent_->getEmail() << "\n";
    std::cout << "College:        " << currentStudent_->getCollege() << "\n";
    std::cout << "Branch:         " << currentStudent_->getBranch() << "\n";
    std::cout << "Track:          " << currentStudent_->getCourse() << "\n";
    std::cout << "Streak:         " << currentStudent_->getStreak() << " days (Best: "
              << currentStudent_->getLongestStreak() << " days)\n";
    std::cout << "Total Score:    " << currentStudent_->getTotalScore() << " points\n";
    std::cout << "Daily Solved:   " << currentStudent_->getDailyChallengesCompleted() << "\n";
    std::cout << "Challenges:     " << currentStudent_->getTotalSolvedCount() << " solved\n";

    std::cout << "\nBadges Earned:\n";
    for (const auto& b : currentStudent_->getBadges()) {
        std::cout << "  * [" << b << "]\n";
    }
    std::cout << "---------------------------------------------------\n";
    std::cout << "[1] Edit College and Branch | [2] Return\n";
    std::cout << "Choose: ";
    int editChoice = 0;
    if (std::cin >> editChoice && editChoice == 1) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "New College: ";
        std::string newColl;
        std::getline(std::cin, newColl);
        std::cout << "New Branch: ";
        std::string newBr;
        std::getline(std::cin, newBr);

        if (!newColl.empty()) currentStudent_->setCollege(newColl);
        if (!newBr.empty()) currentStudent_->setBranch(newBr);
        userRepo_.save(*currentStudent_);
        std::cout << "[*] Profile updated successfully!\n";
    }
}

void ConsoleUI::handleViewLeaderboard() {
    std::cout << "\nSelect Leaderboard Scope:\n";
    std::cout << "  [1] Global | [2] By My College (" << currentStudent_->getCollege()
              << ") | [3] By My Branch (" << currentStudent_->getBranch()
              << ") | [4] By My Track (" << currentStudent_->getCourse() << ")\n";
    std::cout << "Choice (1-4): ";

    int scopeChoice = 1;
    if (!(std::cin >> scopeChoice)) scopeChoice = 1;

    LeaderboardScope scope = LeaderboardScope::GLOBAL;
    std::string target = "";
    if (scopeChoice == 2) {
        scope = LeaderboardScope::BY_COLLEGE;
        target = currentStudent_->getCollege();
    } else if (scopeChoice == 3) {
        scope = LeaderboardScope::BY_BRANCH;
        target = currentStudent_->getBranch();
    } else if (scopeChoice == 4) {
        scope = LeaderboardScope::BY_SUBJECT;
        target = currentStudent_->getCourse();
    }

    std::cout << "\nSelect Ranking Strategy:\n";
    std::cout << "  [1] Rank by Total Score | [2] Rank by Challenges Solved | [3] Rank by Active Streak\n";
    std::cout << "Choice (1-3): ";
    int stratChoice = 1;
    if (!(std::cin >> stratChoice)) stratChoice = 1;

    std::unique_ptr<IRankingStrategy> strategy;
    if (stratChoice == 2) strategy = std::make_unique<RankBySolvedCount>();
    else if (stratChoice == 3) strategy = std::make_unique<RankByStreak>();
    else strategy = std::make_unique<RankByTotalScore>();

    auto allStudents = userRepo_.getAllStudents();
    Leaderboard<LeaderboardEntry> lb;

    for (const auto& s : allStudents) {
        LeaderboardEntry e;
        e.userId = s->getId();
        e.name = s->getName();
        e.course = s->getCourse();
        e.college = s->getCollege();
        e.branch = s->getBranch();
        e.challengesSolved = s->getTotalSolvedCount();
        e.totalScore = s->getTotalScore();
        e.streak = s->getStreak();
        e.earliestSolveTime = s->getLastActiveDate();
        lb.addEntry(e);
    }

    auto filteredList = lb.filter(scope, target);
    Leaderboard<LeaderboardEntry>::sortWithStrategy(filteredList, *strategy);
    Leaderboard<LeaderboardEntry>::display(filteredList, currentStudent_->getId(), 10, strategy->getName());

    int solved = currentStudent_->getTotalSolvedCount();
    std::cout << "Subject Progress [C++ Track]: "
              << Leaderboard<LeaderboardEntry>::renderProgressBar(solved, 10, 12) << "\n\n";
}

// ---------------- Admin Menu Handlers ----------------

void ConsoleUI::showAdminDashboard() {
    std::cout << "\n================ ADMINISTRATOR PANEL ================\n";
    std::cout << " Logged in as: " << currentUser_->getName() << " (Role: " << currentUser_->getRole() << ")\n";
    std::cout << "-----------------------------------------------------\n";
    std::cout << "[1] View User & Subject Analytics\n";
    std::cout << "[2] Search Questions Database\n";
    std::cout << "[3] Add New Question (Validated)\n";
    std::cout << "[4] Soft-Delete / Deactivate Question\n";
    std::cout << "[5] Bulk Import Questions from JSON\n";
    std::cout << "[6] View Reported Questions\n";
    std::cout << "[7] View Difficulty Feedback Discrepancies\n";
    std::cout << "[8] Logout\n";
    std::cout << "Admin action: ";

    int choice = 0;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    switch (choice) {
        case 1: handleAdminStats(); break;
        case 2: handleAdminSearchQuestions(); break;
        case 3: handleAdminAddQuestion(); break;
        case 4: handleAdminSoftDeleteQuestion(); break;
        case 5: handleAdminBulkImport(); break;
        case 6: handleAdminViewReports(); break;
        case 7: handleAdminDifficultyDiscrepancies(); break;
        case 8:
            std::cout << "Logging out administrator...\n";
            currentUser_ = nullptr;
            currentStudent_ = nullptr;
            break;
        default:
            std::cout << "Invalid option.\n";
            break;
    }
}

void ConsoleUI::handleAdminStats() {
    auto stats = adminService_.getUserStats(*currentUser_, streakService_.getCurrentDate());
    std::cout << "\n--- System Analytics ---\n";
    std::cout << "Total Registered Students: " << stats.studentsCount << "\n";
    std::cout << "Students Active Today:     " << stats.activeTodayCount << "\n";
    std::cout << "Attempts per Subject:\n";
    for (const auto& entry : stats.attemptsPerSubject) {
        std::cout << "  - " << entry.first << ": " << entry.second << " attempts\n";
    }
}

void ConsoleUI::handleAdminSearchQuestions() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Search query: ";
    std::string query;
    std::getline(std::cin, query);

    auto results = adminService_.searchQuestions(*currentUser_, query);
    std::cout << "\nFound " << results.size() << " question(s):\n";
    for (const auto& q : results) {
        std::cout << "  [" << q->getId() << "] (" << (q->isActive() ? "ACTIVE" : "INACTIVE") << ") "
                  << "[" << q->getSubject() << "/" << q->getTopic() << "] "
                  << "[" << difficultyToString(q->getDifficulty()) << "] "
                  << q->getText() << "\n";
    }
}

void ConsoleUI::handleAdminAddQuestion() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string id, subject, topic, diffStr, text, exp, optA, optB, answer;

    std::cout << "\n--- Add New Multiple Choice Question ---\n";
    std::cout << "Question ID: "; std::getline(std::cin, id);
    std::cout << "Subject: "; std::getline(std::cin, subject);
    std::cout << "Topic: "; std::getline(std::cin, topic);
    std::cout << "Difficulty (EASY/MEDIUM/HARD): "; std::getline(std::cin, diffStr);
    std::cout << "Question Text: "; std::getline(std::cin, text);
    std::cout << "Option A: "; std::getline(std::cin, optA);
    std::cout << "Option B: "; std::getline(std::cin, optB);
    std::cout << "Correct Answer: "; std::getline(std::cin, answer);
    std::cout << "Explanation: "; std::getline(std::cin, exp);

    Difficulty diff = difficultyFromString(diffStr);
    auto q = std::make_shared<MCQQuestion>(id, subject, topic, diff, text, exp, std::vector<std::string>{optA, optB}, answer);

    try {
        adminService_.addQuestion(*currentUser_, *q);
        std::cout << "[*] Question added successfully!\n";
    } catch (const std::exception& e) {
        std::cout << "[-] Error: " << e.what() << "\n";
    }
}

void ConsoleUI::handleAdminSoftDeleteQuestion() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter Question ID to deactivate: ";
    std::string qId;
    std::getline(std::cin, qId);

    try {
        adminService_.softDeleteQuestion(*currentUser_, qId);
        std::cout << "[*] Question '" << qId << "' deactivated (soft-deleted). Historical attempts remain intact.\n";
    } catch (const std::exception& e) {
        std::cout << "[-] Error: " << e.what() << "\n";
    }
}

void ConsoleUI::handleAdminBulkImport() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter path to JSON questions file: ";
    std::string path;
    std::getline(std::cin, path);

    auto report = adminService_.bulkImportQuestions(*currentUser_, path);
    std::cout << "\n--- Bulk Import Report ---\n";
    std::cout << "Total Read: " << report.totalRead << "\n";
    std::cout << "Accepted:   " << report.accepted << "\n";
    std::cout << "Rejected:   " << report.rejected << "\n";
    for (const auto& err : report.errorDetails) {
        std::cout << "  * " << err << "\n";
    }
}

void ConsoleUI::handleAdminViewReports() {
    auto reports = adminService_.listReportedQuestions(*currentUser_);
    std::cout << "\n--- Reported Questions (" << reports.size() << ") ---\n";
    for (const auto& r : reports) {
        std::cout << "  * [" << r.questionId << "] Reported by " << r.userId
                  << ": " << r.reason << " (" << r.reportedAt << ")\n";
    }
}

void ConsoleUI::handleAdminDifficultyDiscrepancies() {
    auto discs = adminService_.getDifficultyDiscrepancies(*currentUser_);
    std::cout << "\n--- Difficulty Discrepancies (" << discs.size() << ") ---\n";
    for (const auto& d : discs) {
        std::cout << "  * Question " << d.first << ": " << d.second << "\n";
    }
}

// Phase 4: Resume-Based Interview Handler
void ConsoleUI::handleResumeInterview() {
    if (!aiFeedback_->isConfigured()) {
        std::cout << "\n[!] AI Features are disabled. Set AI_API_KEY, AI_PROVIDER, and AI_MODEL environment variables to enable resume-based interviews.\n";
        return;
    }

    if (!usageTracker_->canMakeAiCall(currentStudent_->getId())) {
        std::cout << "\n[!] Daily AI quota exceeded. Please try again tomorrow.\n";
        return;
    }

    // Display privacy consent notice
    std::cout << "\n" << ResumeInterviewMode::getPrivacyConsentNotice() << "\n";
    std::cout << "\nDo you consent to proceeding? [yes/no]: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string consent;
    std::getline(std::cin, consent);
    if (consent != "yes" && consent != "y" && consent != "YES") {
        std::cout << "Resume interview cancelled.\n";
        return;
    }

    std::cout << "\nChoose resume input method:\n";
    std::cout << "[1] Paste resume text directly\n";
    std::cout << "[2] Enter path to a .txt resume file\n";
    std::cout << "Choice: ";
    int method = 0;
    std::cin >> method;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string resumeText;
    std::string errorMsg;

    if (method == 2) {
        std::cout << "Enter file path: ";
        std::string path;
        std::getline(std::cin, path);
        resumeText = ResumeInterviewMode::loadResumeFile(path, errorMsg);
        if (resumeText.empty()) {
            std::cout << "[!] Error: " << errorMsg << "\n";
            return;
        }
    } else {
        std::cout << "Paste your resume text (max 6000 characters) and press Enter twice when done:\n";
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) break;
            resumeText += line + "\n";
            if (resumeText.size() > 6100) break;
        }
        if (!ResumeInterviewMode::validateResumeLength(resumeText, errorMsg)) {
            std::cout << "[!] Error: " << errorMsg << "\n";
            return;
        }
    }

    std::cout << "\n[...] Analyzing your resume with AI (this may take a moment)...\n";
    ResumeAnalyzer analyzer;
    ResumeAnalysisResult result = analyzer.analyzeResume(resumeText, currentStudent_->getCourse());

    if (!result.success) {
        std::cout << "[!] Resume analysis failed: " << result.errorMessage << "\n";
        return;
    }

    // Show extracted summary and get confirmation
    std::cout << "\n=== EXTRACTED PROFILE SUMMARY ===\n";
    std::cout << "Experience: " << result.experienceSummary << "\n";
    std::cout << "Skills: ";
    for (size_t i = 0; i < result.extractedSkills.size() && i < 8; i++) {
        std::cout << result.extractedSkills[i] << (i + 1 < result.extractedSkills.size() ? ", " : "");
    }
    std::cout << "\nProjects: ";
    for (size_t i = 0; i < result.extractedProjects.size() && i < 4; i++) {
        std::cout << result.extractedProjects[i] << (i + 1 < result.extractedProjects.size() ? ", " : "");
    }
    std::cout << "\n" << result.generatedQuestions.size() << " tailored questions generated.\n";
    std::cout << "\nDo you want to start the resume-based interview? [yes/no]: ";
    std::string confirm;
    std::getline(std::cin, confirm);
    if (confirm != "yes" && confirm != "y" && confirm != "YES") {
        std::cout << "Interview cancelled.\n";
        return;
    }

    usageTracker_->recordAiCall(currentStudent_->getId());

    auto session = resumeMode_->createResumeSession(currentStudent_->getId(), result);
    session->start();

    std::cout << "\n=======================================================\n";
    std::cout << "         RESUME-BASED INTERVIEW SESSION\n";
    std::cout << " Questions drawn from YOUR projects and skills.\n";
    std::cout << "=======================================================\n";

    while (!session->isCompleted()) {
        auto q = session->getNextQuestion();
        if (!q) break;

        int qNum = session->getStandardQuestionsAttempted() + 1;
        std::cout << "\n--- Question #" << qNum << " --- [" << q->getTopic() << "] " << difficultyToString(q->getDifficulty()) << "\n";
        q->display();

        int limit = InterviewSession::getTimeLimitForDifficulty(q->getDifficulty());
        std::cout << "\n[Time Limit: " << limit << "s]\nYour answer: ";
        auto t0 = std::chrono::steady_clock::now();
        std::string response;
        std::getline(std::cin, response);
        int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - t0).count());

        bool correct = session->submitAnswer(q, response, elapsed);
        std::cout << (correct ? "[+] Good answer!\n" : "[-] " + q->getExplanation() + "\n");
    }

    session->finish();
    streakService_.recordActivity(*currentStudent_);

    FeedbackReport report = aiFeedback_->generateFeedback(*session);
    std::cout << "\n=======================================================\n";
    std::cout << "          RESUME INTERVIEW FEEDBACK REPORT\n";
    if (!report.notice.empty()) std::cout << " " << report.notice << "\n";
    std::cout << "=======================================================\n";
    std::cout << "Score: " << report.totalScore << "/" << report.maxScore
              << " (" << std::fixed << std::setprecision(1) << report.percentage << "%)\n";
    if (report.isAiGenerated && !report.overallSummary.empty()) {
        std::cout << "\nSummary: " << report.overallSummary << "\n";
    }
    std::cout << "\nRecommendations:\n";
    for (const auto& s : report.suggestions) std::cout << "  * " << s << "\n";
    std::cout << "=======================================================\n";
}

// Phase 4: Answer Review Coaching Handler
void ConsoleUI::handleAnswerReviewCoaching() {
    if (!answerReviewer_->isAvailable()) {
        std::cout << "\n[!] AI Answer Review requires an AI API key. Set AI_API_KEY environment variable.\n";
        return;
    }

    int remaining = usageTracker_->getRemainingReviews(currentStudent_->getId());
    if (remaining <= 0) {
        std::cout << "\n[!] Daily review limit reached (5 reviews/day). Try again tomorrow.\n";
        return;
    }
    std::cout << "\n=== ANSWER REVIEW COACHING === (" << remaining << " reviews remaining today)\n";
    std::cout << "Type the question you want to practice:\n> ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string questionText;
    std::getline(std::cin, questionText);

    std::cout << "Type your answer:\n> ";
    std::string studentAnswer;
    std::getline(std::cin, studentAnswer);

    std::cout << "\n[...] Reviewing your answer with AI...\n";
    auto result = answerReviewer_->reviewAnswer(currentStudent_->getId(), questionText, studentAnswer);

    if (!result.success) {
        std::cout << "[!] Review failed: " << result.errorMessage << "\n";
        return;
    }

    std::cout << "\n--- AI COACHING FEEDBACK ---\n";
    std::cout << "Rating: " << result.rating << "/5\n";
    std::cout << "Feedback: " << result.feedback << "\n";
    if (!result.strengths.empty()) {
        std::cout << "\nStrengths:\n";
        for (const auto& s : result.strengths) std::cout << "  [+] " << s << "\n";
    }
    if (!result.improvements.empty()) {
        std::cout << "\nImprovements:\n";
        for (const auto& i : result.improvements) std::cout << "  [-] " << i << "\n";
    }
    std::cout << "\nImproved Model Answer:\n" << result.improvedAnswer << "\n";
    std::cout << "-----------------------------\n";
}
