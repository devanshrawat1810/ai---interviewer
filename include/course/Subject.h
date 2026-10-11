#pragma once

#include <string>
#include <vector>
#include "course/Topic.h"

// OOP Concept: Composition
// Subject composes multiple Topic instances and defines a syllabus or subject track.
class Subject {
private:
    std::string id_;
    std::string name_;
    std::string description_;
    std::vector<Topic> topics_;

public:
    Subject(std::string id, std::string name, std::string description = "");

    void addTopic(Topic topic);

    const std::string& getId() const noexcept;
    const std::string& getName() const noexcept;
    const std::string& getDescription() const noexcept;
    const std::vector<Topic>& getTopics() const noexcept;
};
