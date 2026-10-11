#include "core/TextQuestion.h"
#include <iostream>
#include <algorithm>
#include <cctype>

// OOP Concept: Polymorphism & Inheritance
// Implements keyword-based answer scoring for open-ended text questions.

TextQuestion::TextQuestion(std::string id,
                           std::string subject,
                           std::string topic,
                           Difficulty difficulty,
                           std::string text,
                           std::string explanation,
                           std::string sampleAnswer,
                           std::vector<std::string> keywords,
                           double threshold)
    : Question(std::move(id),
               std::move(subject),
               std::move(topic),
               difficulty,
               "text",
               std::move(text),
               std::move(explanation)),
      sampleAnswer_(std::move(sampleAnswer)),
      keywords_(std::move(keywords)),
      keywordMatchThreshold_(threshold) {}

void TextQuestion::display() const {
    std::cout << "\n[" << difficultyToString(difficulty_) << "] " << text_ << "\n";
    std::cout << "  (Provide a concise explanation in your own words)\n";
}

bool TextQuestion::evaluate(const std::string& userResponse) const {
    if (userResponse.empty() || keywords_.empty()) return false;

    // Convert response to lowercase
    std::string lowerResponse = userResponse;
    std::transform(lowerResponse.begin(), lowerResponse.end(), lowerResponse.begin(), ::tolower);

    int matchedCount = 0;
    for (const auto& keyword : keywords_) {
        std::string lowerKey = keyword;
        std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
        if (lowerResponse.find(lowerKey) != std::string::npos) {
            ++matchedCount;
        }
    }

    double ratio = static_cast<double>(matchedCount) / static_cast<double>(keywords_.size());
    return ratio >= keywordMatchThreshold_;
}

const std::string& TextQuestion::getSampleAnswer() const noexcept {
    return sampleAnswer_;
}

const std::vector<std::string>& TextQuestion::getKeywords() const noexcept {
    return keywords_;
}

double TextQuestion::getThreshold() const noexcept {
    return keywordMatchThreshold_;
}
