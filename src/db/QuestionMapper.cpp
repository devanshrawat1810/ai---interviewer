#include "db/QuestionMapper.h"
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include "core/CodingQuestion.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/types.hpp>
#endif

// OOP Concept: Single Responsibility Principle & Polymorphic Mapping
// Implements serialization and deserialization for the Question class hierarchy with soft delete support.

nlohmann::json QuestionMapper::toJson(const Question& question) {
    nlohmann::json j;
    j["id"] = question.getId();
    j["subject"] = question.getSubject();
    j["topic"] = question.getTopic();
    j["difficulty"] = difficultyToString(question.getDifficulty());
    j["type"] = question.getType();
    j["text"] = question.getText();
    j["explanation"] = question.getExplanation();
    j["isActive"] = question.isActive();

    if (const auto* mcq = dynamic_cast<const MCQQuestion*>(&question)) {
        j["options"] = mcq->getOptions();
        j["answer"] = mcq->getCorrectAnswer();
    } else if (const auto* txt = dynamic_cast<const TextQuestion*>(&question)) {
        j["sampleAnswer"] = txt->getSampleAnswer();
        j["keywords"] = txt->getKeywords();
        j["threshold"] = txt->getThreshold();
    } else if (const auto* code = dynamic_cast<const CodingQuestion*>(&question)) {
        j["starterCode"] = code->getStarterCode();
        j["points"] = code->getPoints();
        nlohmann::json tcArr = nlohmann::json::array();
        for (const auto& tc : code->getTestCases()) {
            tcArr.push_back({
                {"input", tc.input},
                {"expectedOutput", tc.expectedOutput},
                {"isHidden", tc.isHidden}
            });
        }
        j["testCases"] = tcArr;
    }

    return j;
}

std::shared_ptr<Question> QuestionMapper::fromJson(const nlohmann::json& j) {
    std::string id = j.value("id", "");
    std::string subject = j.value("subject", "");
    std::string topic = j.value("topic", "");
    Difficulty diff = difficultyFromString(j.value("difficulty", "EASY"));
    std::string type = j.value("type", "mcq");
    std::string text = j.value("text", "");
    std::string explanation = j.value("explanation", "");
    bool isActive = j.value("isActive", true);

    std::shared_ptr<Question> q;

    if (type == "mcq") {
        std::vector<std::string> options;
        if (j.contains("options") && j["options"].is_array()) {
            options = j["options"].get<std::vector<std::string>>();
        }
        std::string answer = j.value("answer", "");
        q = std::make_shared<MCQQuestion>(id, subject, topic, diff, text, explanation, options, answer);
    } else if (type == "text") {
        std::string sample = j.value("sampleAnswer", j.value("answer", ""));
        std::vector<std::string> keywords;
        if (j.contains("keywords") && j["keywords"].is_array()) {
            keywords = j["keywords"].get<std::vector<std::string>>();
        }
        double threshold = j.value("threshold", 0.5);
        q = std::make_shared<TextQuestion>(id, subject, topic, diff, text, explanation, sample, keywords, threshold);
    } else if (type == "coding") {
        std::string starter = j.value("starterCode", "");
        int points = j.value("points", 100);
        std::vector<TestCase> testCases;
        if (j.contains("testCases") && j["testCases"].is_array()) {
            for (const auto& tcJson : j["testCases"]) {
                TestCase tc;
                tc.input = tcJson.value("input", "");
                tc.expectedOutput = tcJson.value("expectedOutput", "");
                tc.isHidden = tcJson.value("isHidden", false);
                testCases.push_back(tc);
            }
        }
        q = std::make_shared<CodingQuestion>(id, subject, topic, diff, text, explanation, starter, testCases, points);
    } else {
        q = std::make_shared<TextQuestion>(id, subject, topic, diff, text, explanation, "", std::vector<std::string>{});
    }

    if (q) {
        q->setActive(isActive);
    }
    return q;
}

#ifdef AI_INTERVIEWER_HAS_MONGODB
bsoncxx::document::value QuestionMapper::toBson(const Question& question) {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::open_document;
    using bsoncxx::builder::stream::close_document;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::finalize;

    document builder;
    builder << "_id" << question.getId()
            << "subject" << question.getSubject()
            << "topic" << question.getTopic()
            << "difficulty" << difficultyToString(question.getDifficulty())
            << "type" << question.getType()
            << "text" << question.getText()
            << "explanation" << question.getExplanation()
            << "isActive" << question.isActive();

    if (const auto* mcq = dynamic_cast<const MCQQuestion*>(&question)) {
        auto optArr = builder << "options" << open_array;
        for (const auto& opt : mcq->getOptions()) {
            optArr << opt;
        }
        optArr << close_array;
        builder << "answer" << mcq->getCorrectAnswer();
    } else if (const auto* txt = dynamic_cast<const TextQuestion*>(&question)) {
        builder << "sampleAnswer" << txt->getSampleAnswer();
        auto keyArr = builder << "keywords" << open_array;
        for (const auto& k : txt->getKeywords()) {
            keyArr << k;
        }
        keyArr << close_array;
        builder << "threshold" << txt->getThreshold();
    } else if (const auto* code = dynamic_cast<const CodingQuestion*>(&question)) {
        builder << "starterCode" << code->getStarterCode()
                << "points" << code->getPoints();
        auto tcArr = builder << "testCases" << open_array;
        for (const auto& tc : code->getTestCases()) {
            tcArr << open_document
                  << "input" << tc.input
                  << "expectedOutput" << tc.expectedOutput
                  << "isHidden" << tc.isHidden
                  << close_document;
        }
        tcArr << close_array;
    }

    return builder << finalize;
}

std::shared_ptr<Question> QuestionMapper::fromBson(const bsoncxx::document::view& doc) {
    std::string id = doc["_id"].get_string().value.to_string();
    std::string subject = doc["subject"].get_string().value.to_string();
    std::string topic = doc["topic"].get_string().value.to_string();
    Difficulty diff = difficultyFromString(doc["difficulty"].get_string().value.to_string());
    std::string type = doc["type"].get_string().value.to_string();
    std::string text = doc["text"].get_string().value.to_string();
    std::string explanation = doc["explanation"] ? doc["explanation"].get_string().value.to_string() : "";
    bool isActive = doc["isActive"] ? doc["isActive"].get_bool().value : true;

    std::shared_ptr<Question> q;

    if (type == "mcq") {
        std::vector<std::string> options;
        if (doc["options"]) {
            for (const auto& elem : doc["options"].get_array().value) {
                options.push_back(elem.get_string().value.to_string());
            }
        }
        std::string answer = doc["answer"] ? doc["answer"].get_string().value.to_string() : "";
        q = std::make_shared<MCQQuestion>(id, subject, topic, diff, text, explanation, options, answer);
    } else if (type == "text") {
        std::string sample = doc["sampleAnswer"] ? doc["sampleAnswer"].get_string().value.to_string() : "";
        std::vector<std::string> keywords;
        if (doc["keywords"]) {
            for (const auto& elem : doc["keywords"].get_array().value) {
                keywords.push_back(elem.get_string().value.to_string());
            }
        }
        double threshold = doc["threshold"] ? doc["threshold"].get_double().value : 0.5;
        q = std::make_shared<TextQuestion>(id, subject, topic, diff, text, explanation, sample, keywords, threshold);
    } else if (type == "coding") {
        std::string starter = doc["starterCode"] ? doc["starterCode"].get_string().value.to_string() : "";
        int points = doc["points"] ? doc["points"].get_int32().value : 100;
        std::vector<TestCase> testCases;
        if (doc["testCases"]) {
            for (const auto& elem : doc["testCases"].get_array().value) {
                auto tcDoc = elem.get_document().value;
                TestCase tc;
                tc.input = tcDoc["input"].get_string().value.to_string();
                tc.expectedOutput = tcDoc["expectedOutput"].get_string().value.to_string();
                tc.isHidden = tcDoc["isHidden"] ? tcDoc["isHidden"].get_bool().value : false;
                testCases.push_back(tc);
            }
        }
        q = std::make_shared<CodingQuestion>(id, subject, topic, diff, text, explanation, starter, testCases, points);
    } else {
        q = std::make_shared<TextQuestion>(id, subject, topic, diff, text, explanation, "", std::vector<std::string>{});
    }

    if (q) {
        q->setActive(isActive);
    }
    return q;
}
#endif
