#pragma once

#include <QPushButton>
#include <QString>

// OOP Concept: Inheritance & Specialization
// Reusable primary action button with indigo background and focus styling.
class PrimaryButton : public QPushButton {
    Q_OBJECT

public:
    explicit PrimaryButton(const QString& text, QWidget* parent = nullptr);
    void setLoading(bool loading);

private:
    QString originalText_;
    bool isLoading_{false};
};
