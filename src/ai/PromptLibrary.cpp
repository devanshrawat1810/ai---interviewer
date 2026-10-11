#include "ai/PromptLibrary.h"
#include <fstream>
#include <sstream>
#include <algorithm>

PromptLibrary::PromptLibrary(std::string promptsDirectory)
    : promptsDirectory_(std::move(promptsDirectory)) {}

std::string PromptLibrary::getFallbackTemplate(const std::string& templateName) const {
    if (templateName == "feedback.txt" || templateName == "feedback") {
        return "Return JSON only with overallSummary, strengths, weaknesses, perTopicNotes, communicationClarity (1-5), answerStructure (1-5), confidenceCues (1-5), suggestions.\n"
               "Student Records:\n<<<UNTRUSTED_STUDENT_DATA_START>>>\n{{INTERVIEW_RECORDS}}\n<<<UNTRUSTED_STUDENT_DATA_END>>>";
    }
    if (templateName == "followup.txt" || templateName == "followup") {
        return "Return JSON only: {\"followUpQuestion\": \"...\", \"contextReason\": \"...\", \"expectedKeywords\": [...]}\n"
               "Answer:\n<<<UNTRUSTED_STUDENT_DATA_START>>>\n{{STUDENT_ANSWER}}\n<<<UNTRUSTED_STUDENT_DATA_END>>>";
    }
    if (templateName == "resume_questions.txt" || templateName == "resume_questions") {
        return "Return JSON only: {\"extractedSkills\": [], \"extractedProjects\": [], \"experienceSummary\": \"...\", \"questions\": []}\n"
               "Resume:\n<<<UNTRUSTED_RESUME_DATA_START>>>\n{{RESUME_TEXT}}\n<<<UNTRUSTED_RESUME_DATA_END>>>";
    }
    if (templateName == "answer_review.txt" || templateName == "answer_review") {
        return "Return JSON only: {\"rating\": 4, \"feedback\": \"...\", \"improvedAnswer\": \"...\", \"strengths\": [], \"improvements\": []}\n"
               "Answer:\n<<<UNTRUSTED_STUDENT_DATA_START>>>\n{{STUDENT_ANSWER}}\n<<<UNTRUSTED_STUDENT_DATA_END>>>";
    }
    return "{{INPUT}}";
}

std::string PromptLibrary::render(const std::string& templateName,
                                  const std::map<std::string, std::string>& variables) {
    std::string key = templateName;
    if (key.size() < 4 || key.substr(key.size() - 4) != ".txt") {
        key += ".txt";
    }

    std::string content;
    auto it = cachedTemplates_.find(key);
    if (it != cachedTemplates_.end()) {
        content = it->second;
    } else {
        std::string fullPath = promptsDirectory_ + "/" + key;
        std::ifstream file(fullPath);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            content = buffer.str();
            cachedTemplates_[key] = content;
        } else {
            content = getFallbackTemplate(templateName);
        }
    }

    // Replace {{VAR}} placeholders
    for (const auto& kv : variables) {
        std::string placeholder = "{{" + kv.first + "}}";
        size_t pos = 0;
        while ((pos = content.find(placeholder, pos)) != std::string::npos) {
            content.replace(pos, placeholder.length(), kv.second);
            pos += kv.second.length();
        }
    }

    return content;
}

std::string PromptLibrary::sanitizeInput(const std::string& input, size_t maxChars) {
    std::string cleaned;
    cleaned.reserve(std::min(input.size(), maxChars));

    for (char c : input) {
        if (cleaned.size() >= maxChars) break;
        // Strip control characters except newline and tab
        if ((c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == '\t') {
            cleaned.push_back(c);
        }
    }
    return cleaned;
}

std::string PromptLibrary::wrapUntrusted(const std::string& input, const std::string& tag) {
    return "<<<" + tag + "_START>>>\n" + sanitizeInput(input) + "\n<<<" + tag + "_END>>>";
}
