#include "gui/Navigator.h"
#include "gui/ThemeManager.h"
#include "core/Student.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

Navigator::Navigator(QWidget* parent) : QStackedWidget(parent) {}

void Navigator::registerView(const QString& name, QWidget* view) {
    view->setProperty("screenName", name);
    addWidget(view);
}

void Navigator::navigateTo(const QString& name) {
    for (int i = 0; i < count(); ++i) {
        QWidget* w = widget(i);
        if (w->property("screenName").toString() == name) {
            history_.push(name);
            animateTransition(w);
            emit screenChanged(name);
            return;
        }
    }
}

void Navigator::goBack() {
    if (history_.size() > 1) {
        history_.pop(); // Pop current
        QString prevName = history_.top();
        history_.pop(); // Will be re-pushed by navigateTo
        navigateTo(prevName);
    }
}

void Navigator::setCurrentUser(std::shared_ptr<User> user) {
    currentUser_ = std::move(user);
    if (currentUser_) {
        emit userLoggedIn(currentUser_);
    }
}

std::shared_ptr<Student> Navigator::currentStudent() const {
    return std::dynamic_pointer_cast<Student>(currentUser_);
}

void Navigator::logout() {
    currentUser_ = nullptr;
    while (!history_.empty()) history_.pop();
    emit userLoggedOut();
    navigateTo("login");
}

void Navigator::animateTransition(QWidget* nextWidget) {
    if (ThemeManager::instance().isReduceMotion() || currentWidget() == nullptr) {
        setCurrentWidget(nextWidget);
        return;
    }

    auto* effect = new QGraphicsOpacityEffect(nextWidget);
    nextWidget->setGraphicsEffect(effect);
    setCurrentWidget(nextWidget);

    auto* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(200);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    connect(anim, &QPropertyAnimation::finished, [nextWidget]() {
        nextWidget->setGraphicsEffect(nullptr);
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
