#include "gui/views/MainWindow.h"
#include "gui/ServiceContext.h"
#include "gui/Navigator.h"
#include "gui/ThemeManager.h"
#include "gui/presenters/LoginPresenter.h"
#include "gui/presenters/DashboardPresenter.h"
#include "gui/presenters/CourseSelectPresenter.h"
#include "gui/presenters/InterviewPresenter.h"
#include "gui/views/LoginView.h"
#include "gui/views/DashboardView.h"
#include "gui/views/CourseSelectView.h"
#include "gui/views/InterviewView.h"
#include "gui/views/FeedbackView.h"
#include "gui/views/ProfileView.h"
#include "gui/views/LeaderboardView.h"
#include "gui/views/AdminView.h"
#include "gui/views/ResumeView.h"
#include "interview/SessionFactory.h"
#include "core/Student.h"
#include "core/Admin.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

MainWindow::MainWindow(std::shared_ptr<ServiceContext> ctx, QWidget* parent)
    : QMainWindow(parent), ctx_(std::move(ctx)) {
    setMinimumSize(1000, 650);
    setWindowTitle("AI Interviewer — Placement Preparation & Mock Interview Engine");

    setupNavigation();
    setupMenuBar();
}

void MainWindow::setupNavigation() {
    navigator_ = new Navigator(this);
    setCentralWidget(navigator_);

    // Presenters
    loginPresenter_ = new LoginPresenter(ctx_, this);
    dashboardPresenter_ = new DashboardPresenter(ctx_, this);
    courseSelectPresenter_ = new CourseSelectPresenter(ctx_, this);
    interviewPresenter_ = new InterviewPresenter(ctx_, this);

    // Views
    auto* loginView = new LoginView(loginPresenter_, navigator_);
    auto* dashboardView = new DashboardView(dashboardPresenter_, navigator_);
    auto* courseView = new CourseSelectView(courseSelectPresenter_, navigator_);
    auto* interviewView = new InterviewView(interviewPresenter_, navigator_);
    auto* feedbackView = new FeedbackView(navigator_);
    auto* profileView = new ProfileView(navigator_);
    auto* leaderboardView = new LeaderboardView(ctx_, navigator_);
    auto* adminView = new AdminView(ctx_, navigator_);
    auto* resumeView = new ResumeView(ctx_, navigator_);

    navigator_->registerView("login", loginView);
    navigator_->registerView("dashboard", dashboardView);
    navigator_->registerView("course", courseView);
    navigator_->registerView("interview", interviewView);
    navigator_->registerView("feedback", feedbackView);
    navigator_->registerView("profile", profileView);
    navigator_->registerView("leaderboard", leaderboardView);
    navigator_->registerView("admin", adminView);
    navigator_->registerView("resume", resumeView);

    // Authentication Transition
    connect(loginView, &LoginView::authenticated, this, [this, dashboardView](std::shared_ptr<User> user) {
        navigator_->setCurrentUser(user);
        dashboardPresenter_->loadDashboard(user);
        navigator_->navigateTo("dashboard");
    });

    // Dashboard Buttons
    connect(dashboardView, &DashboardView::startInterviewClicked, this, [this]() {
        auto student = navigator_->currentStudent();
        if (student) {
            auto pool = ctx_->questionRepo->getAll();
            auto session = SessionFactory::createSession(
                SessionType::MOCK, student->getId(), "Object-Oriented Programming", pool);
            interviewPresenter_->startSession(std::shared_ptr<InterviewSession>(std::move(session)));
            navigator_->navigateTo("interview");
        }
    });

    connect(dashboardView, &DashboardView::startPracticeClicked, this, [this]() {
        auto student = navigator_->currentStudent();
        if (student) {
            auto pool = ctx_->questionRepo->getAll();
            auto session = SessionFactory::createSession(
                SessionType::PRACTICE, student->getId(), "Data Structures", pool);
            interviewPresenter_->startSession(std::shared_ptr<InterviewSession>(std::move(session)));
            navigator_->navigateTo("interview");
        }
    });

    connect(dashboardView, &DashboardView::resumeInterviewClicked, this, [this]() {
        navigator_->navigateTo("resume");
    });

    connect(dashboardView, &DashboardView::dailyChallengeClicked, this, [this]() {
        auto student = navigator_->currentStudent();
        if (student) {
            auto pool = ctx_->questionRepo->getAll();
            auto session = SessionFactory::createSession(
                SessionType::DAILY, student->getId(), "Daily Challenge", pool);
            interviewPresenter_->startSession(std::shared_ptr<InterviewSession>(std::move(session)));
            navigator_->navigateTo("interview");
        }
    });

    connect(dashboardView, &DashboardView::browseCoursesClicked, this, [this, courseView]() {
        auto student = navigator_->currentStudent();
        if (student) {
            courseSelectPresenter_->loadCourseData(
                QString::fromStdString(student->getCourse()),
                QString::fromStdString(student->getId()));
        } else {
            courseSelectPresenter_->loadCourseData("CSE", "");
        }
        navigator_->navigateTo("course");
    });

    connect(courseView, &CourseSelectView::subjectSelected, this, [this](const QString& subject, const QString& mode) {
        auto student = navigator_->currentStudent();
        std::string uid = student ? student->getId() : "guest_user";
        auto pool = ctx_->questionRepo->getAll();
        SessionType type = (mode == "mock") ? SessionType::MOCK : SessionType::PRACTICE;
        auto session = SessionFactory::createSession(type, uid, subject.toStdString(), pool);
        interviewPresenter_->startSession(std::shared_ptr<InterviewSession>(std::move(session)));
        navigator_->navigateTo("interview");
    });

    connect(profileView, &ProfileView::profileUpdated, this, [this](const QString& college, const QString& branch) {
        auto student = navigator_->currentStudent();
        if (student) {
            // Update student credentials in user repository
            student->setCollege(college.toStdString());
            student->setBranch(branch.toStdString());
            ctx_->userRepo->save(*student);
            QMessageBox::information(this, "Profile Saved", "Your college and department details have been updated successfully!");
        }
    });

    connect(dashboardView, &DashboardView::leaderboardClicked, this, [this, leaderboardView]() {
        auto user = navigator_->currentUser();
        leaderboardView->refreshData(user ? QString::fromStdString(user->getId()) : "");
        navigator_->navigateTo("leaderboard");
    });

    connect(dashboardView, &DashboardView::profileClicked, this, [this, profileView]() {
        profileView->setStudentData(navigator_->currentStudent());
        navigator_->navigateTo("profile");
    });

    connect(dashboardView, &DashboardView::adminPanelClicked, this, [this, adminView]() {
        auto admin = std::dynamic_pointer_cast<Admin>(navigator_->currentUser());
        if (admin) {
            adminView->refreshAdminData(admin);
            navigator_->navigateTo("admin");
        }
    });

    connect(dashboardView, &DashboardView::logoutClicked, navigator_, &Navigator::logout);

    // Back Buttons
    connect(courseView, &CourseSelectView::backRequested, this, [this]() { navigator_->navigateTo("dashboard"); });
    connect(profileView, &ProfileView::backRequested, this, [this]() { navigator_->navigateTo("dashboard"); });
    connect(leaderboardView, &LeaderboardView::backRequested, this, [this]() { navigator_->navigateTo("dashboard"); });
    connect(adminView, &AdminView::backRequested, this, [this]() { navigator_->navigateTo("dashboard"); });
    connect(resumeView, &ResumeView::backRequested, this, [this]() { navigator_->navigateTo("dashboard"); });
    connect(interviewView, &InterviewView::leaveRequested, this, [this]() { navigator_->navigateTo("dashboard"); });

    // Resume Flow launch
    connect(resumeView, &ResumeView::resumeSessionReady, this, [this](std::shared_ptr<InterviewSession> session) {
        interviewPresenter_->startSession(session);
        navigator_->navigateTo("interview");
    });

    // Interview Completion
    connect(interviewView, &InterviewView::interviewFinished, this, [this, feedbackView](const FeedbackReport& report) {
        feedbackView->setReport(report);
        navigator_->navigateTo("feedback");
    });

    connect(feedbackView, &FeedbackView::returnToDashboardClicked, this, [this]() {
        dashboardPresenter_->loadDashboard(navigator_->currentUser());
        navigator_->navigateTo("dashboard");
    });

    connect(feedbackView, &FeedbackView::retryInterviewClicked, this, [this]() {
        auto student = navigator_->currentStudent();
        if (student) {
            auto pool = ctx_->questionRepo->getAll();
            auto session = SessionFactory::createSession(
                SessionType::MOCK, student->getId(), "Object-Oriented Programming", pool);
            interviewPresenter_->startSession(std::shared_ptr<InterviewSession>(std::move(session)));
            navigator_->navigateTo("interview");
        }
    });

    navigator_->navigateTo("login");
}

void MainWindow::setupMenuBar() {
    auto* menuBar = this->menuBar();

    // View Menu
    auto* viewMenu = menuBar->addMenu("&View");
    auto* lightAction = viewMenu->addAction("Light Theme");
    auto* darkAction = viewMenu->addAction("Dark Theme");
    viewMenu->addSeparator();
    auto* reduceMotionAction = viewMenu->addAction("Reduce Motion");
    reduceMotionAction->setCheckable(true);
    reduceMotionAction->setChecked(ThemeManager::instance().isReduceMotion());

    connect(lightAction, &QAction::triggered, []() {
        ThemeManager::instance().setTheme(ThemeManager::ThemeMode::Light);
    });
    connect(darkAction, &QAction::triggered, []() {
        ThemeManager::instance().setTheme(ThemeManager::ThemeMode::Dark);
    });
    connect(reduceMotionAction, &QAction::toggled, [](bool checked) {
        ThemeManager::instance().setReduceMotion(checked);
    });

    // Help Menu
    auto* helpMenu = menuBar->addMenu("&Help");
    auto* aboutAction = helpMenu->addAction("About AI Interviewer");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About AI Interviewer",
            "<h3>AI Interviewer v1.0.0</h3>"
            "<p>A C++17 Mock Placement Interview & Coding Simulation Platform.</p>"
            "<p>Built using Qt 6 Widgets, Model-View-Presenter (MVP), and modern C++ design patterns.</p>"
            "<p><b>Authors:</b> Placement Preparation & Computer Science Department</p>");
    });
}
