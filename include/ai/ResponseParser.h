#pragma once

#include "ai/AiExceptions.h"
#include "ai/ILLMClient.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// OOP Concept: Single Responsibility Principle & Fault Tolerance
// Parses, sanitizes, and validates LLM JSON outputs with automated self-healing repair retries.
class ResponseParser {
public:
    static std::string stripCodeFences(const std::string& text);

    static nlohmann::json parseJson(const std::string& rawText);

    static void validateRequiredFields(const nlohmann::json& j,
                                       const std::vector<std::string>& requiredFields);

    static void validateIntRange(const nlohmann::json& j,
                                 const std::string& field,
                                 int minVal,
                                 int maxVal);

    // Attempts parsing. If it fails, executes exactly ONE repair query to the LLM.
    static nlohmann::json parseWithAutoRepair(ILLMClient& client,
                                             const std::string& rawResponse,
                                             const std::string& schemaPrompt,
                                             const std::vector<std::string>& requiredFields);
};
