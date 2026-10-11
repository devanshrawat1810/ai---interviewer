#include "course/Subject.h"
#include <utility>

// OOP Concept: Composition
// Implementation of Subject class owning Topic objects.

Subject::Subject(std::string id, std::string name, std::string description)
    : id_(std::move(id)),
      name_(std::move(name)),
      description_(std::move(description)) {}

void Subject::addTopic(Topic topic) {
    topics_.push_back(std::move(topic));
}

const std::string& Subject::getId() const noexcept {
    return id_;
}

const std::string& Subject::getName() const noexcept {
    return name_;
}

const std::string& Subject::getDescription() const noexcept {
    return description_;
}

const std::vector<Topic>& Subject::getTopics() const noexcept {
    return topics_;
}
