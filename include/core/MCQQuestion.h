#pragma once

#include "core/Question.h"
#include <vector>
#include <string>

// OOP Concept: Inheritance & Polymorphism
// Specializes the abstract Question base class for multiple-choice formats.
// Overrides display() and evaluate() to handle options and letter/index comparisons.
class MCQQuestion : public Question {
private:
    std::vector<std::string> options_;
    std::string correctAnswer_;

public:
    MCQQuestion(std::string id,
                std::string subject,
                std::string topic,
                Difficulty difficulty,
                std::string text,
                std::string explanation,
                std::vector<std::string> options,
                std::string correctAnswer);

    ~MCQQuestion() override = default;

    void display() const override;
    bool evaluate(const std::string& userResponse) const override;

    const std::vector<std::string>& getOptions() const noexcept;
    const std::string& getCorrectAnswer() const noexcept;
};
