#include "ai/LLMClientFactory.h"
#include "ai/AnthropicClient.h"
#include "ai/OpenAICompatibleClient.h"
#include "ai/MockLLMClient.h"
#include "ai/NullLLMClient.h"
#include <cstdlib>
#include <algorithm>

// OOP Concept: Factory Pattern Implementation
// Decouples client instantiation from caller logic and enforces safe fallbacks.

static std::string getEnvVar(const char* name, const std::string& defaultValue = "") {
#if defined(_MSC_VER)
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, name) == 0 && buf != nullptr) {
        std::string val(buf);
        free(buf);
        return val;
    }
    return defaultValue;
#else
    const char* val = std::getenv(name);
    return val ? std::string(val) : defaultValue;
#endif
}

std::shared_ptr<ILLMClient> LLMClientFactory::createClientFromEnv() {
    std::string demoMode = getEnvVar("DEMO_MODE", "0");
    if (demoMode == "1" || demoMode == "true" || demoMode == "TRUE") {
        std::string cannedFeedback = R"({
            "overall_summary": "[DEMO MODE] Excellent command of Object-Oriented principles, clean algorithmic structure, and clear communication.",
            "strengths": ["Quick recall of polymorphism and inheritance", "Clean code organization and modular thinking"],
            "weaknesses": ["Could elaborate more on edge case handling in concurrent scenarios"],
            "communication_clarity": 5,
            "answer_structure": 5,
            "confidence_cues": 4,
            "suggestions": ["Review memory models in C++20", "Practice more complex graph algorithms", "Keep up your daily consistency!"]
        })";
        return std::make_shared<MockLLMClient>(cannedFeedback);
    }

    std::string provider = getEnvVar("AI_PROVIDER", "openai");
    std::string apiKey = getEnvVar("AI_API_KEY", "");
    std::string model = getEnvVar("AI_MODEL", "");
    std::string baseUrl = getEnvVar("AI_BASE_URL", "");
    std::string timeoutStr = getEnvVar("AI_TIMEOUT_SECONDS", "30");

    int timeoutSeconds = 30;
    try {
        if (!timeoutStr.empty()) {
            timeoutSeconds = std::stoi(timeoutStr);
        }
    } catch (...) {
        timeoutSeconds = 30;
    }

    return createClient(provider, apiKey, model, baseUrl, timeoutSeconds);
}

std::shared_ptr<ILLMClient> LLMClientFactory::createClient(const std::string& provider,
                                                           const std::string& apiKey,
                                                           const std::string& model,
                                                           const std::string& baseUrl,
                                                           int timeoutSeconds) {
    std::string provLower = provider;
    std::transform(provLower.begin(), provLower.end(), provLower.begin(), ::tolower);

    // Mock client for testing bypasses apiKey requirement
    if (provLower == "mock") {
        return std::make_shared<MockLLMClient>();
    }

    // No key configured: return NullLLMClient
    if (apiKey.empty()) {
        return std::make_shared<NullLLMClient>();
    }

    if (provLower == "anthropic" || provLower == "claude") {
        std::string m = model.empty() ? "claude-3-5-sonnet-20241022" : model;
        return std::make_shared<AnthropicClient>(apiKey, m, baseUrl, timeoutSeconds);
    }

    if (provLower == "gemini") {
        std::string m = model.empty() ? "gemini-1.5-flash" : model;
        std::string url = baseUrl.empty() ? "https://generativelanguage.googleapis.com/v1beta/openai/chat/completions" : baseUrl;
        return std::make_shared<OpenAICompatibleClient>(apiKey, m, url, timeoutSeconds);
    }

    // Default to OpenAI / OpenAI-compatible
    std::string m = model.empty() ? "gpt-4o-mini" : model;
    return std::make_shared<OpenAICompatibleClient>(apiKey, m, baseUrl, timeoutSeconds);
}
