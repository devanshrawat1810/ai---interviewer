#pragma once

#include "core/Question.h"
#include "ai/ILLMClient.h"
#include "ai/PromptLibrary.h"
#include <memory>
#include <string>
#include <vector>

struct ResumeAnalysisResult {
    std::vector<std::string> extractedSkills;
    std::vector<std::string> extractedProjects;
    std::string experienceSummary;
    std::vector<std::shared_ptr<Question>> generatedQuestions;
    bool success{false};
    std::string errorMessage;
};

// OOP Concept: Single Responsibility & Factory Pattern
// Parses resumes via LLM, extracts verified candidate competencies, and generates customized Question instances.
class ResumeAnalyzer {
private:
    std::shared_ptr<ILLMClient> llmClient_;
    std::shared_ptr<PromptLibrary> promptLibrary_;

public:
    explicit ResumeAnalyzer(std::shared_ptr<ILLMClient> llmClient = nullptr,
                           std::shared_ptr<PromptLibrary> promptLib = nullptr);

    ResumeAnalysisResult analyzeResume(const std::string& resumeText,
                                       const std::string& subject = "Resume Interview");

    bool isAvailable() const noexcept;
};
