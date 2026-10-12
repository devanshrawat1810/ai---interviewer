#pragma once

#include <QStackedWidget>
#include <memory>
#include <stack>
#include <QString>

class User;
class Student;

// OOP Concept: Navigation Controller & State Machine
// Manages screen transitions, view stack, animations, and active session state.
class Navigator : public QStackedWidget {
    Q_OBJECT

public:
    explicit Navigator(QWidget* parent = nullptr);
    ~Navigator() override = default;

    void registerView(const QString& name, QWidget* view);
    void navigateTo(const QString& name);
    void goBack();

    void setCurrentUser(std::shared_ptr<User> user);
    std::shared_ptr<User> currentUser() const noexcept { return currentUser_; }
    std::shared_ptr<Student> currentStudent() const;

    void logout();

signals:
    void userLoggedIn(std::shared_ptr<User> user);
    void userLoggedOut();
    void screenChanged(const QString& name);

private:
    std::shared_ptr<User> currentUser_;
    std::stack<QString> history_;
    void animateTransition(QWidget* nextWidget);
};
