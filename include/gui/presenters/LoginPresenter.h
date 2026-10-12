#pragma once

#include <QObject>
#include <memory>
#include <QString>
#include "gui/Result.h"

class ServiceContext;
class User;

// OOP Concept: Presenter in Model-View-Presenter (Observer Pattern)
// Pure QObject orchestrating authentication logic without GUI dependencies.
class LoginPresenter : public QObject {
    Q_OBJECT

public:
    explicit LoginPresenter(std::shared_ptr<ServiceContext> ctx, QObject* parent = nullptr);

    void login(const QString& email, const QString& password);
    void registerUser(const QString& name, const QString& email, const QString& password,
                      const QString& course, const QString& college, const QString& branch);

signals:
    void loginStarted();
    void loginSucceeded(std::shared_ptr<User> user);
    void loginFailed(const QString& friendlyError);

    void registerStarted();
    void registerSucceeded(std::shared_ptr<User> user);
    void registerFailed(const QString& friendlyError);

private:
    std::shared_ptr<ServiceContext> ctx_;
};
