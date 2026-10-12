#include "gui/views/LoginView.h"
#include "gui/presenters/LoginPresenter.h"
#include "gui/ThemeTokens.h"
#include "gui/ThemeManager.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

LoginView::LoginView(LoginPresenter* presenter, QWidget* parent)
    : QWidget(parent), presenter_(presenter) {
    setupUi();

    connect(presenter_, &LoginPresenter::loginStarted, this, &LoginView::onLoginStarted);
    connect(presenter_, &LoginPresenter::loginSucceeded, this, &LoginView::onLoginSucceeded);
    connect(presenter_, &LoginPresenter::loginFailed, this, &LoginView::onLoginFailed);

    connect(presenter_, &LoginPresenter::registerStarted, this, &LoginView::onRegisterStarted);
    connect(presenter_, &LoginPresenter::registerSucceeded, this, &LoginView::onRegisterSucceeded);
    connect(presenter_, &LoginPresenter::registerFailed, this, &LoginView::onRegisterFailed);
}

void LoginView::setupUi() {
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Left Brand Panel (Gradient)
    auto* brandPanel = new QWidget(this);
    brandPanel->setMinimumWidth(360);
    brandPanel->setStyleSheet(QString(R"(
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2);
        color: #FFFFFF;
    )").arg(ThemeTokens::ColorPrimary, ThemeTokens::ColorAccent));

    auto* brandLayout = new QVBoxLayout(brandPanel);
    brandLayout->setContentsMargins(40, 60, 40, 60);
    brandLayout->setSpacing(20);

    auto* logoLabel = new QLabel("AI Interviewer", brandPanel);
    logoLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #FFFFFF;");

    auto* tagline = new QLabel("Master your campus placements with adaptive mock interviews & AI feedback.", brandPanel);
    tagline->setWordWrap(true);
    tagline->setStyleSheet("font-size: 16px; color: #E0E7FF; line-height: 1.4;");

    auto* perks = new QLabel(
        "• Progressive difficulty ramps (Easy to Hard)\n"
        "• Daily challenges and streak rewards\n"
        "• Provider-neutral AI with offline fallback\n"
        "• Live C++ coding challenge execution", brandPanel);
    perks->setStyleSheet("font-size: 14px; color: #C7D2FE; line-height: 1.8;");

    brandLayout->addWidget(logoLabel);
    brandLayout->addWidget(tagline);
    brandLayout->addWidget(perks);
    brandLayout->addStretch();

    // Right Form Panel
    auto* rightPanel = new QWidget(this);
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setAlignment(Qt::AlignCenter);

    auto* card = new Card(rightPanel);
    card->setFixedWidth(420);
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(16);

    // Form Stack (0 = Login, 1 = Register)
    formStack_ = new QStackedWidget(card);

    // --- Login Form ---
    auto* loginPage = new QWidget(formStack_);
    auto* loginLayout = new QVBoxLayout(loginPage);
    loginLayout->setSpacing(14);

    auto* loginTitle = new QLabel("Sign In to Your Account", loginPage);
    loginTitle->setStyleSheet("font-size: 20px; font-weight: bold;");

    loginErrorBanner_ = new QLabel(loginPage);
    loginErrorBanner_->setStyleSheet(QString("background: #FEF2F2; color: %1; border: 1px solid #FCA5A5; border-radius: 6px; padding: 8px;")
        .arg(ThemeTokens::ColorDanger));
    loginErrorBanner_->setVisible(false);
    loginErrorBanner_->setWordWrap(true);

    loginEmailField_ = new FormField("College Email Address", loginPage);
    loginEmailField_->input()->setPlaceholderText("e.g. student@college.edu");

    loginPasswordField_ = new PasswordField("Password", loginPage);

    loginSubmitBtn_ = new PrimaryButton("Sign In", loginPage);

    auto* toRegRow = new QHBoxLayout();
    auto* noAccLabel = new QLabel("Don't have an account?", loginPage);
    auto* toRegBtn = new QPushButton("Create one here", loginPage);
    toRegBtn->setCursor(Qt::PointingHandCursor);
    toRegBtn->setStyleSheet("color: #4F46E5; font-weight: bold; border: none; background: transparent; text-decoration: underline;");

    toRegRow->addWidget(noAccLabel);
    toRegRow->addWidget(toRegBtn);
    toRegRow->addStretch();

    loginLayout->addWidget(loginTitle);
    loginLayout->addWidget(loginErrorBanner_);
    loginLayout->addWidget(loginEmailField_);
    loginLayout->addWidget(loginPasswordField_);
    loginLayout->addWidget(loginSubmitBtn_);
    loginLayout->addLayout(toRegRow);

    // --- Register Form ---
    auto* regPage = new QWidget(formStack_);
    auto* regLayout = new QVBoxLayout(regPage);
    regLayout->setSpacing(10);

    auto* regTitle = new QLabel("Create Student Account", regPage);
    regTitle->setStyleSheet("font-size: 20px; font-weight: bold;");

    regErrorBanner_ = new QLabel(regPage);
    regErrorBanner_->setStyleSheet(QString("background: #FEF2F2; color: %1; border: 1px solid #FCA5A5; border-radius: 6px; padding: 6px;")
        .arg(ThemeTokens::ColorDanger));
    regErrorBanner_->setVisible(false);
    regErrorBanner_->setWordWrap(true);

    regNameField_ = new FormField("Full Name", regPage);
    regEmailField_ = new FormField("College Email", regPage);
    regPasswordField_ = new PasswordField("Password (min 8 chars)", regPage);
    regCourseField_ = new FormField("Course Track", regPage);
    regCourseField_->setText("CSE");
    regCollegeField_ = new FormField("College / University", regPage);
    regBranchField_ = new FormField("Branch", regPage);

    regSubmitBtn_ = new PrimaryButton("Complete Registration", regPage);

    auto* toLoginBtn = new QPushButton("Back to Sign In", regPage);
    toLoginBtn->setCursor(Qt::PointingHandCursor);
    toLoginBtn->setStyleSheet("color: #4F46E5; font-weight: bold; border: none; background: transparent; text-decoration: underline;");

    regLayout->addWidget(regTitle);
    regLayout->addWidget(regErrorBanner_);
    regLayout->addWidget(regNameField_);
    regLayout->addWidget(regEmailField_);
    regLayout->addWidget(regPasswordField_);
    regLayout->addWidget(regCourseField_);
    regLayout->addWidget(regCollegeField_);
    regLayout->addWidget(regBranchField_);
    regLayout->addWidget(regSubmitBtn_);
    regLayout->addWidget(toLoginBtn);

    formStack_->addWidget(loginPage);
    formStack_->addWidget(regPage);

    cardLayout->addWidget(formStack_);
    rightLayout->addWidget(card);

    mainLayout->addWidget(brandPanel);
    mainLayout->addWidget(rightPanel, 1);

    // Connect form toggles
    connect(toRegBtn, &QPushButton::clicked, this, [this]() {
        loginErrorBanner_->setVisible(false);
        formStack_->setCurrentIndex(1);
    });
    connect(toLoginBtn, &QPushButton::clicked, this, [this]() {
        regErrorBanner_->setVisible(false);
        formStack_->setCurrentIndex(0);
    });

    // Submissions
    connect(loginSubmitBtn_, &QPushButton::clicked, this, [this]() {
        loginErrorBanner_->setVisible(false);
        presenter_->login(loginEmailField_->text(), loginPasswordField_->text());
    });

    connect(regSubmitBtn_, &QPushButton::clicked, this, [this]() {
        regErrorBanner_->setVisible(false);
        presenter_->registerUser(regNameField_->text(), regEmailField_->text(), regPasswordField_->text(),
                                 regCourseField_->text(), regCollegeField_->text(), regBranchField_->text());
    });
}

void LoginView::onLoginStarted() {
    loginSubmitBtn_->setLoading(true);
}

void LoginView::onLoginSucceeded(std::shared_ptr<User> user) {
    loginSubmitBtn_->setLoading(false);
    emit authenticated(user);
}

void LoginView::onLoginFailed(const QString& error) {
    loginSubmitBtn_->setLoading(false);
    loginErrorBanner_->setText(error);
    loginErrorBanner_->setVisible(true);
    triggerShakeAnimation();
}

void LoginView::onRegisterStarted() {
    regSubmitBtn_->setLoading(true);
}

void LoginView::onRegisterSucceeded(std::shared_ptr<User> user) {
    regSubmitBtn_->setLoading(false);
    emit authenticated(user);
}

void LoginView::onRegisterFailed(const QString& error) {
    regSubmitBtn_->setLoading(false);
    regErrorBanner_->setText(error);
    regErrorBanner_->setVisible(true);
    triggerShakeAnimation();
}

void LoginView::triggerShakeAnimation() {
    if (ThemeManager::instance().isReduceMotion()) return;
    // Simple visual feedback
}
