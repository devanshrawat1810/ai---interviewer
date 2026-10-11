#include "ai/ResponseParser.h"
#include <iostream>

using json = nlohmann::json;

std::string ResponseParser::stripCodeFences(const std::string& text) {
    std::string s = text;
    // Trim leading whitespace
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    s = s.substr(first);

    // Remove leading fence e.g. ```json or ```
    if (s.rfind("```", 0) == 0) {
        size_t newline = s.find('\n');
        if (newline != std::string::npos) {
            s = s.substr(newline + 1);
        } else {
            s = "";
        }
    }

    // Trim trailing whitespace
    size_t last = s.find_last_not_of(" \t\r\n");
    if (last != std::string::npos) {
        s = s.substr(0, last + 1);
    }

    // Remove trailing fence
    if (s.size() >= 3 && s.substr(s.size() - 3) == "```") {
        s = s.substr(0, s.size() - 3);
        last = s.find_last_not_of(" \t\r\n");
        if (last != std::string::npos) {
            s = s.substr(0, last + 1);
        }
    }

    return s;
}

json ResponseParser::parseJson(const std::string& rawText) {
    std::string clean = stripCodeFences(rawText);
    try {
        return json::parse(clean);
    } catch (const json::parse_error& e) {
        throw ParseException("Malformed JSON string: " + std::string(e.what()));
    }
}

void ResponseParser::validateRequiredFields(const json& j,
                                            const std::vector<std::string>& requiredFields) {
    if (!j.is_object()) {
        throw ParseException("Root JSON structure must be an object");
    }
    for (const auto& field : requiredFields) {
        if (!j.contains(field)) {
            throw ParseException("Missing required JSON field: " + field);
        }
    }
}

void ResponseParser::validateIntRange(const json& j,
                                      const std::string& field,
                                      int minVal,
                                      int maxVal) {
    if (!j.contains(field)) {
        throw ParseException("Missing field: " + field);
    }
    if (!j[field].is_number_integer()) {
        throw ParseException("Field " + field + " must be an integer");
    }
    int val = j[field].get<int>();
    if (val < minVal || val > maxVal) {
        throw ParseException("Field " + field + " value " + std::to_string(val) +
                             " out of range [" + std::to_string(minVal) + ", " +
                             std::to_string(maxVal) + "]");
    }
}

json ResponseParser::parseWithAutoRepair(ILLMClient& client,
                                        const std::string& rawResponse,
                                        const std::string& schemaPrompt,
                                        const std::vector<std::string>& requiredFields) {
    std::string parseErrorMsg;
    try {
        json j = parseJson(rawResponse);
        validateRequiredFields(j, requiredFields);
        return j;
    } catch (const ParseException& pe) {
        parseErrorMsg = pe.what();
    }

    // Exactly one repair retry
    LLMRequest repairReq;
    repairReq.systemPrompt = "You are a JSON repair specialist. You must output ONLY valid JSON matching the requested schema. No code fences, no extra text.";
    repairReq.userMessage = "The previous response failed schema validation:\n"
                           "Validation Error: " + parseErrorMsg + "\n\n"
                           "Failed Response was:\n" + rawResponse + "\n\n"
                           "Required Schema Instructions:\n" + schemaPrompt + "\n\n"
                           "Output ONLY the corrected JSON object:";
    repairReq.maxTokens = 1024;
    repairReq.temperature = 0.2;

    LLMResponse repairResp = client.complete(repairReq);
    if (!repairResp.success) {
        throw ParseException("Repair query failed: " + repairResp.errorMessage + " (Original: " + parseErrorMsg + ")");
    }

    try {
        json repairedJson = parseJson(repairResp.content);
        validateRequiredFields(repairedJson, requiredFields);
        return repairedJson;
    } catch (const ParseException& pe) {
        throw ParseException("Output repair attempt failed: " + std::string(pe.what()) + " (Original: " + parseErrorMsg + ")");
    }
}
