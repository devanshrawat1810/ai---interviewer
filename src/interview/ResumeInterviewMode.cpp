#include "interview/ResumeInterviewMode.h"
#include "interview/LinearRampStrategy.h"
#include <fstream>
#include <sstream>

ResumeInterviewMode::ResumeInterviewMode(std::shared_ptr<ResumeAnalyzer> analyzer)
    : analyzer_(analyzer ? std::move(analyzer) : std::make_shared<ResumeAnalyzer>()) {}

std::string ResumeInterviewMode::getPrivacyConsentNotice() {
    return "=========================================================\n"
           "                RESUME PRIVACY NOTICE\n"
           "=========================================================\n"
           "Your resume text will be processed by our AI service to\n"
           "extract technical competencies and formulate customized\n"
           "mock interview questions.\n\n"
           "PRIVACY GUARANTEE:\n"
           "- Your raw resume text is NEVER stored on our servers or disk.\n"
           "- Only verified extracted skill tags are kept on your profile.\n"
           "- Input is transmitted securely over TLS encryption.\n"
           "=========================================================";
}

bool ResumeInterviewMode::validateResumeLength(const std::string& text, std::string& outErrorMessage) {
    if (text.empty()) {
        outErrorMessage = "Resume text cannot be empty.";
        return false;
    }
    if (text.size() > 6000) {
        outErrorMessage = "Resume exceeds 6000 character limit (length: " +
                          std::to_string(text.size()) + " characters). Please provide a concise summary.";
        return false;
    }
    return true;
}

std::string ResumeInterviewMode::loadResumeFile(const std::string& filePath, std::string& outErrorMessage) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        outErrorMessage = "Could not open resume file: " + filePath;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    if (!validateResumeLength(content, outErrorMessage)) {
        return "";
    }

    return content;
}

std::unique_ptr<InterviewSession> ResumeInterviewMode::createResumeSession(const std::string& userId,
                                                                          const ResumeAnalysisResult& analysis) {
    static int resumeSessionCounter = 0;
    std::string sessionId = "resume_session_" + std::to_string(++resumeSessionCounter);

    auto strategy = std::make_unique<LinearRampStrategy>();
    int targetCount = static_cast<int>(analysis.generatedQuestions.size());
    if (targetCount > 10) targetCount = 10;
    if (targetCount < 1) targetCount = 1;

    return std::make_unique<InterviewSession>(
        sessionId,
        userId,
        "Resume Practice Track",
        "resume",
        std::move(strategy),
        analysis.generatedQuestions,
        targetCount
    );
}
