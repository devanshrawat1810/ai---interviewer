#include "core/MCQQuestion.h"
#include <iostream>
#include <algorithm>
#include <cctype>

// OOP Concept: Polymorphism & Inheritance
// Concrete implementation of multiple choice question behavior.

MCQQuestion::MCQQuestion(std::string id,
                         std::string subject,
                         std::string topic,
                         Difficulty difficulty,
                         std::string text,
                         std::string explanation,
                         std::vector<std::string> options,
                         std::string correctAnswer)
    : Question(std::move(id),
               std::move(subject),
               std::move(topic),
               difficulty,
               "mcq",
               std::move(text),
               std::move(explanation)),
      options_(std::move(options)),
      correctAnswer_(std::move(correctAnswer)) {}

void MCQQuestion::display() const {
    std::cout << "\n[" << difficultyToString(difficulty_) << "] " << text_ << "\n";
    char label = 'A';
    for (const auto& opt : options_) {
        std::cout << "  " << label++ << ") " << opt << "\n";
    }
}

bool MCQQuestion::evaluate(const std::string& userResponse) const {
    if (userResponse.empty()) return false;

    // Normalize response: trim leading/trailing whitespace
    std::string resp = userResponse;
    resp.erase(resp.begin(), std::find_if(resp.begin(), resp.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    resp.erase(std::find_if(resp.rbegin(), resp.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), resp.end());

    if (resp.empty()) return false;

    // Check letter match: 'A', 'B', 'C', etc.
    if (resp.length() == 1) {
        char c = static_cast<char>(std::toupper(static_cast<unsigned char>(resp[0])));
        int index = c - 'A';
        if (index >= 0 && index < static_cast<int>(options_.size())) {
            // Compare chosen option with correctAnswer_
            std::string chosen = options_[static_cast<size_t>(index)];
            if (chosen == correctAnswer_) return true;

            // Or if correctAnswer_ is the letter itself ("A")
            if (correctAnswer_.length() == 1 &&
                std::toupper(static_cast<unsigned char>(correctAnswer_[0])) == c) {
                return true;
            }
        }
    }

    // Direct text equality (case-insensitive)
    std::string normUser = resp;
    std::string normCorrect = correctAnswer_;
    std::transform(normUser.begin(), normUser.end(), normUser.begin(), ::tolower);
    std::transform(normCorrect.begin(), normCorrect.end(), normCorrect.begin(), ::tolower);

    return normUser == normCorrect;
}

const std::vector<std::string>& MCQQuestion::getOptions() const noexcept {
    return options_;
}

const std::string& MCQQuestion::getCorrectAnswer() const noexcept {
    return correctAnswer_;
}
