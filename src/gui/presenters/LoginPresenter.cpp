#include "gui/presenters/LoginPresenter.h"
#include "gui/ServiceContext.h"
#include "gui/ErrorTranslator.h"
#include <QtConcurrent>

LoginPresenter::LoginPresenter(std::shared_ptr<ServiceContext> ctx, QObject* parent)
    : QObject(parent), ctx_(std::move(ctx)) {}

void LoginPresenter::login(const QString& email, const QString& password) {
    emit loginStarted();

    std::string e = email.trimmed().toStdString();
    std::string p = password.toStdString();

    // Run off UI thread with QtConcurrent
    (void)QtConcurrent::run([this, e, p]() {
        try {
            auto user = ctx_->authService->login(e, p);
            if (user) {
                QMetaObject::invokeMethod(this, [this, user]() {
                    emit loginSucceeded(user);
                }, Qt::QueuedConnection);
            } else {
                QMetaObject::invokeMethod(this, [this]() {
                    emit loginFailed("Incorrect email or password. Please try again.");
                }, Qt::QueuedConnection);
            }
        } catch (const std::exception& ex) {
            QString err = QString::fromStdString(ErrorTranslator::translate(ex));
            QMetaObject::invokeMethod(this, [this, err]() {
                emit loginFailed(err);
            }, Qt::QueuedConnection);
        }
    });
}

void LoginPresenter::registerUser(const QString& name, const QString& email, const QString& password,
                                  const QString& course, const QString& college, const QString& branch) {
    emit registerStarted();

    std::string n = name.trimmed().toStdString();
    std::string e = email.trimmed().toStdString();
    std::string p = password.toStdString();
    std::string c = course.trimmed().toStdString();
    std::string clg = college.trimmed().toStdString();
    std::string b = branch.trimmed().toStdString();

    (void)QtConcurrent::run([this, n, e, p, c, clg, b]() {
        try {
            auto user = ctx_->authService->registerStudent(n, e, p, c, clg, b);
            if (user) {
                QMetaObject::invokeMethod(this, [this, user]() {
                    emit registerSucceeded(user);
                }, Qt::QueuedConnection);
            } else {
                QMetaObject::invokeMethod(this, [this]() {
                    emit registerFailed("Registration could not be completed. Please try again.");
                }, Qt::QueuedConnection);
            }
        } catch (const std::exception& ex) {
            QString err = QString::fromStdString(ErrorTranslator::translate(ex));
            QMetaObject::invokeMethod(this, [this, err]() {
                emit registerFailed(err);
            }, Qt::QueuedConnection);
        }
    });
}
