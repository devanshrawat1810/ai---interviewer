#pragma once

#include "core/Question.h"
#include "ai/ILLMClient.h"
#include "ai/PromptLibrary.h"
#include <memory>
#include <string>

// OOP Concept: Single Responsibility & Factory Method
// Analyzes qualifying candidate responses to dynamically generate contextual follow-up questions.
class FollowUpGenerator {
private:
    std::shared_ptr<ILLMClient> llmClient_;
    std::shared_ptr<PromptLibrary> promptLibrary_;

public:
    explicit FollowUpGenerator(std::shared_ptr<ILLMClient> llmClient = nullptr,
                               std::shared_ptr<PromptLibrary> promptLib = nullptr);

    // Generates a dynamic TextQuestion follow-up, or nullptr on failure/unavailable
    std::shared_ptr<Question> generateFollowUp(const std::string& originalQuestionText,
                                              const std::string& studentAnswer,
                                              const std::string& subject = "General",
                                              Difficulty diff = Difficulty::MEDIUM);

    bool isAvailable() const noexcept;
};
