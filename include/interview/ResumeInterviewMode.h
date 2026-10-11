#pragma once

#include "ai/ResumeAnalyzer.h"
#include "interview/InterviewSession.h"
#include "core/Student.h"
#include <memory>
#include <string>

// OOP Concept: Orchestrator & Separation of Concerns
// Manages privacy consent, resume text validation, file loading, and session creation for resume interviews.
class ResumeInterviewMode {
private:
    std::shared_ptr<ResumeAnalyzer> analyzer_;

public:
    explicit ResumeInterviewMode(std::shared_ptr<ResumeAnalyzer> analyzer = nullptr);

    static std::string getPrivacyConsentNotice();

    // Validates resume character count (<= 6000 chars)
    static bool validateResumeLength(const std::string& text, std::string& outErrorMessage);

    // Reads file content from path (rejects files > 6000 chars)
    static std::string loadResumeFile(const std::string& filePath, std::string& outErrorMessage);

    // Creates an InterviewSession from confirmed resume questions
    std::unique_ptr<InterviewSession> createResumeSession(const std::string& userId,
                                                          const ResumeAnalysisResult& analysis);
};
