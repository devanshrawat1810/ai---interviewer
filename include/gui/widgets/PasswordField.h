#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

// OOP Concept: Composite Component & UX Security
// Password input with show/hide toggle and CapsLock status feedback.
class PasswordField : public QWidget {
    Q_OBJECT

public:
    explicit PasswordField(const QString& labelText, QWidget* parent = nullptr);

    QLineEdit* input() const noexcept { return input_; }
    QString text() const { return input_->text(); }
    void setText(const QString& t) { input_->setText(t); }

    void setError(const QString& errorText);
    void clearError();

private:
    QLabel* label_;
    QLineEdit* input_;
    QPushButton* toggleBtn_;
    QLabel* errorLabel_;
    bool isVisible_{false};
};
