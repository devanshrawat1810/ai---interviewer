#include "ai/OpenAICompatibleClient.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

// OOP Concept: Concrete Strategy Implementation
// Interacts with standard OpenAI chat/completions API format.

OpenAICompatibleClient::OpenAICompatibleClient(std::string apiKey,
                                               std::string model,
                                               std::string baseUrl,
                                               int timeoutSeconds,
                                               std::shared_ptr<HttpClient> httpClient)
    : apiKey_(std::move(apiKey)),
      model_(std::move(model)),
      baseUrl_(std::move(baseUrl)),
      timeoutSeconds_(timeoutSeconds),
      httpClient_(httpClient ? std::move(httpClient) : std::make_shared<HttpClient>()) {
    if (baseUrl_.empty()) {
        baseUrl_ = "https://api.openai.com/v1/chat/completions";
    }
}

LLMResponse OpenAICompatibleClient::complete(const LLMRequest& request) {
    std::vector<std::string> headers = {
        "Authorization: Bearer " + apiKey_,
        "Content-Type: application/json"
    };

    json payload;
    payload["model"] = model_;
    json messages = json::array();
    if (!request.systemPrompt.empty()) {
        messages.push_back({{"role", "system"}, {"content", request.systemPrompt}});
    }
    messages.push_back({{"role", "user"}, {"content", request.userMessage}});

    payload["messages"] = messages;
    payload["max_tokens"] = request.maxTokens;
    payload["temperature"] = request.temperature;

    try {
        HttpResponse resp = httpClient_->post(baseUrl_, headers, payload.dump(), timeoutSeconds_);
        json parsed = json::parse(resp.body);

        LLMResponse response;
        if (parsed.contains("choices") && parsed["choices"].is_array() && !parsed["choices"].empty()) {
            const auto& choice = parsed["choices"][0];
            if (choice.contains("message") && choice["message"].contains("content")) {
                response.content = choice["message"]["content"].get<std::string>();
            }
        } else if (parsed.contains("error")) {
            response.success = false;
            response.errorMessage = parsed["error"].value("message", "OpenAI API error");
            return response;
        }

        if (parsed.contains("usage") && parsed["usage"].is_object()) {
            response.promptTokens = parsed["usage"].value("prompt_tokens", 0);
            response.completionTokens = parsed["usage"].value("completion_tokens", 0);
        }
        response.success = true;
        return response;
    } catch (const std::exception& ex) {
        LLMResponse err;
        err.success = false;
        err.errorMessage = ex.what();
        return err;
    }
}

std::string OpenAICompatibleClient::getProviderName() const {
    return "OpenAI-Compatible (" + model_ + ")";
}
