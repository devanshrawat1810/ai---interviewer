#include "course/Topic.h"
#include <utility>

// OOP Concept: Encapsulation
// Implementation of Topic class.

Topic::Topic(std::string id, std::string name, std::string description)
    : id_(std::move(id)),
      name_(std::move(name)),
      description_(std::move(description)) {}

const std::string& Topic::getId() const noexcept {
    return id_;
}

const std::string& Topic::getName() const noexcept {
    return name_;
}

const std::string& Topic::getDescription() const noexcept {
    return description_;
}
