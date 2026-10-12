#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QString>

// OOP Concept: Composite Component
// Standard form entry with integrated label, text box, and inline validation error text.
class FormField : public QWidget {
    Q_OBJECT

public:
    explicit FormField(const QString& labelText, QWidget* parent = nullptr);

    QLineEdit* input() const noexcept { return input_; }
    QString text() const { return input_->text(); }
    void setText(const QString& t) { input_->setText(t); }

    void setError(const QString& errorText);
    void clearError();

private:
    QLabel* label_;
    QLineEdit* input_;
    QLabel* errorLabel_;
};
