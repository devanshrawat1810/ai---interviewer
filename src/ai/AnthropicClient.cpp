#include "ai/AnthropicClient.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

// OOP Concept: Concrete Strategy Implementation
// Interacts with Anthropic Messages API format (v1 / 2023-06-01).

AnthropicClient::AnthropicClient(std::string apiKey,
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
        baseUrl_ = "https://api.anthropic.com/v1/messages";
    }
}

LLMResponse AnthropicClient::complete(const LLMRequest& request) {
    std::vector<std::string> headers = {
        "x-api-key: " + apiKey_,
        "anthropic-version: 2023-06-01",
        "content-type: application/json"
    };

    json payload;
    payload["model"] = model_;
    if (!request.systemPrompt.empty()) {
        payload["system"] = request.systemPrompt;
    }
    payload["messages"] = json::array({
        {{"role", "user"}, {"content", request.userMessage}}
    });
    payload["max_tokens"] = request.maxTokens;
    payload["temperature"] = request.temperature;

    try {
        HttpResponse resp = httpClient_->post(baseUrl_, headers, payload.dump(), timeoutSeconds_);
        json parsed = json::parse(resp.body);

        LLMResponse response;
        if (parsed.contains("content") && parsed["content"].is_array() && !parsed["content"].empty()) {
            response.content = parsed["content"][0].value("text", "");
        } else if (parsed.contains("error")) {
            response.success = false;
            response.errorMessage = parsed["error"].value("message", "Anthropic error");
            return response;
        }

        if (parsed.contains("usage") && parsed["usage"].is_object()) {
            response.promptTokens = parsed["usage"].value("input_tokens", 0);
            response.completionTokens = parsed["usage"].value("output_tokens", 0);
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

std::string AnthropicClient::getProviderName() const {
    return "Anthropic (" + model_ + ")";
}
