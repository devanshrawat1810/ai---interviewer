#pragma once

#include <QMainWindow>
#include <memory>

class ServiceContext;
class Navigator;
class LoginPresenter;
class DashboardPresenter;
class CourseSelectPresenter;
class InterviewPresenter;

// OOP Concept: Top-Level Presentation Window
// Hosts Navigator, manages theme toggling, menu actions, and clean application shutdown.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(std::shared_ptr<ServiceContext> ctx, QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    std::shared_ptr<ServiceContext> ctx_;
    Navigator* navigator_;

    LoginPresenter* loginPresenter_;
    DashboardPresenter* dashboardPresenter_;
    CourseSelectPresenter* courseSelectPresenter_;
    InterviewPresenter* interviewPresenter_;

    void setupNavigation();
    void setupMenuBar();
};
