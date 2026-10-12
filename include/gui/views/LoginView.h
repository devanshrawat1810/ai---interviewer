#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "gui/widgets/Card.h"
#include "gui/widgets/FormField.h"
#include "gui/widgets/PasswordField.h"
#include "gui/widgets/PrimaryButton.h"

class LoginPresenter;
class User;

// OOP Concept: Passive View in Model-View-Presenter
// Emits user input signals and binds strictly to presenter responses.
class LoginView : public QWidget {
    Q_OBJECT

public:
    explicit LoginView(LoginPresenter* presenter, QWidget* parent = nullptr);

public slots:
    void onLoginStarted();
    void onLoginSucceeded(std::shared_ptr<User> user);
    void onLoginFailed(const QString& error);

    void onRegisterStarted();
    void onRegisterSucceeded(std::shared_ptr<User> user);
    void onRegisterFailed(const QString& error);

signals:
    void authenticated(std::shared_ptr<User> user);

private:
    LoginPresenter* presenter_;

    // Login Form Widgets
    FormField* loginEmailField_;
    PasswordField* loginPasswordField_;
    PrimaryButton* loginSubmitBtn_;
    QLabel* loginErrorBanner_;

    // Register Form Widgets
    FormField* regNameField_;
    FormField* regEmailField_;
    PasswordField* regPasswordField_;
    FormField* regCourseField_;
    FormField* regCollegeField_;
    FormField* regBranchField_;
    PrimaryButton* regSubmitBtn_;
    QLabel* regErrorBanner_;

    QStackedWidget* formStack_;
    void setupUi();
    void triggerShakeAnimation();
};
