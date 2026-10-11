#pragma once

#include <string>
#include <map>

// OOP Concept: Encapsulation & Separation of Concerns
// Manages prompt template retrieval, variable substitution, and prompt-injection safety sanitization.
class PromptLibrary {
private:
    std::string promptsDirectory_{"prompts"};
    std::map<std::string, std::string> cachedTemplates_;

    std::string getFallbackTemplate(const std::string& templateName) const;

public:
    explicit PromptLibrary(std::string promptsDirectory = "prompts");

    std::string render(const std::string& templateName,
                       const std::map<std::string, std::string>& variables);

    // Prompt injection defense sanitization
    static std::string sanitizeInput(const std::string& input, size_t maxChars = 6000);
    static std::string wrapUntrusted(const std::string& input, const std::string& tag = "UNTRUSTED_STUDENT_DATA");
};
