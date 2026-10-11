#pragma once

#include "core/Question.h"
#include <vector>
#include <string>

// OOP Concept: Inheritance & Polymorphism
// Specializes Question for free-form textual answers.
// Overrides evaluate() to perform keyword extraction and scoring.
class TextQuestion : public Question {
private:
    std::string sampleAnswer_;
    std::vector<std::string> keywords_;
    double keywordMatchThreshold_; // Default e.g. 0.5 (at least 50% keywords matched)

public:
    TextQuestion(std::string id,
                 std::string subject,
                 std::string topic,
                 Difficulty difficulty,
                 std::string text,
                 std::string explanation,
                 std::string sampleAnswer,
                 std::vector<std::string> keywords,
                 double threshold = 0.5);

    ~TextQuestion() override = default;

    void display() const override;
    bool evaluate(const std::string& userResponse) const override;

    const std::string& getSampleAnswer() const noexcept;
    const std::vector<std::string>& getKeywords() const noexcept;
    double getThreshold() const noexcept;
};
