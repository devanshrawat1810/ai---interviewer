#include "core/Question.h"
#include <utility>

// OOP Concept: Abstraction
// Implements base Question member accessors and constructor.

Question::Question(std::string id,
                   std::string subject,
                   std::string topic,
                   Difficulty difficulty,
                   std::string type,
                   std::string text,
                   std::string explanation,
                   bool isActive)
    : id_(std::move(id)),
      subject_(std::move(subject)),
      topic_(std::move(topic)),
      difficulty_(difficulty),
      type_(std::move(type)),
      text_(std::move(text)),
      explanation_(std::move(explanation)),
      isActive_(isActive) {}

const std::string& Question::getId() const noexcept { return id_; }
const std::string& Question::getSubject() const noexcept { return subject_; }
const std::string& Question::getTopic() const noexcept { return topic_; }
Difficulty Question::getDifficulty() const noexcept { return difficulty_; }
const std::string& Question::getType() const noexcept { return type_; }
const std::string& Question::getText() const noexcept { return text_; }
const std::string& Question::getExplanation() const noexcept { return explanation_; }
bool Question::isActive() const noexcept { return isActive_; }

void Question::setActive(bool active) noexcept { isActive_ = active; }
void Question::setText(std::string text) { text_ = std::move(text); }
void Question::setExplanation(std::string explanation) { explanation_ = std::move(explanation); }
