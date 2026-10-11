#pragma once

#include <string>
#include <vector>
#include "course/Subject.h"

// OOP Concept: Composition
// Course composes multiple Subject objects into an academic major or curriculum.
class Course {
private:
    std::string id_;
    std::string name_;
    std::string description_;
    std::vector<Subject> subjects_;

public:
    Course(std::string id, std::string name, std::string description = "");

    void addSubject(Subject subject);

    const std::string& getId() const noexcept;
    const std::string& getName() const noexcept;
    const std::string& getDescription() const noexcept;
    const std::vector<Subject>& getSubjects() const noexcept;
    const Subject* findSubject(const std::string& subjectId) const;

    // Factory helper preloading default college courses: CSE, ECE, Commerce, Common
    static std::vector<Course> getPreloadedCourses();
};
