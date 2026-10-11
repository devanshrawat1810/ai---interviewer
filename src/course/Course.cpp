#include "course/Course.h"
#include <utility>

// OOP Concept: Composition & Factory Preload
// Implementation of Course class containing Subject instances.

Course::Course(std::string id, std::string name, std::string description)
    : id_(std::move(id)),
      name_(std::move(name)),
      description_(std::move(description)) {}

void Course::addSubject(Subject subject) {
    subjects_.push_back(std::move(subject));
}

const std::string& Course::getId() const noexcept {
    return id_;
}

const std::string& Course::getName() const noexcept {
    return name_;
}

const std::string& Course::getDescription() const noexcept {
    return description_;
}

const std::vector<Subject>& Course::getSubjects() const noexcept {
    return subjects_;
}

const Subject* Course::findSubject(const std::string& subjectId) const {
    for (const auto& s : subjects_) {
        if (s.getId() == subjectId) {
            return &s;
        }
    }
    return nullptr;
}

std::vector<Course> Course::getPreloadedCourses() {
    std::vector<Course> courses;

    // 1. CSE - Computer Science & Engineering
    {
        Course cse("CSE", "Computer Science & Engineering", "Software Engineering, OOP, Systems & Data Structures");
        
        Subject cppOop("cpp_oop", "C++ & Object-Oriented Programming", "Encapsulation, Polymorphism, Inheritance, Abstraction");
        cppOop.addTopic(Topic("classes_objects", "Classes & Objects", "Constructors, destructors, member visibility"));
        cppOop.addTopic(Topic("polymorphism", "Polymorphism & Virtual Tables", "Virtual functions, pure virtual, vptr, dynamic binding"));
        cppOop.addTopic(Topic("memory_stl", "Memory Management & STL", "Smart pointers, STL containers, iterators"));
        cppOop.addTopic(Topic("design_patterns", "Design Patterns", "Factory, Singleton, Strategy, Observer"));
        cse.addSubject(cppOop);

        Subject dsa("dsa", "Data Structures & Algorithms", "Arrays, Linked Lists, Trees, Graphs, Sorting");
        dsa.addTopic(Topic("linear_ds", "Linear Data Structures", "Arrays, Stacks, Queues, Linked Lists"));
        dsa.addTopic(Topic("trees_graphs", "Trees & Graphs", "BST, Traversals, Graph BFS/DFS"));
        cse.addSubject(dsa);

        courses.push_back(cse);
    }

    // 2. ECE - Electronics & Communication Engineering
    {
        Course ece("ECE", "Electronics & Communication Engineering", "Digital Circuits, Microcontrollers & Embedded Systems");
        
        Subject embedded("embedded", "Embedded Systems & C", "Microcontroller architecture, low-level I/O, RTOS");
        embedded.addTopic(Topic("microcontrollers", "Microcontroller Architecture", "ARM, timers, interrupts"));
        embedded.addTopic(Topic("comm_protocols", "Protocols", "UART, SPI, I2C"));
        ece.addSubject(embedded);

        courses.push_back(ece);
    }

    // 3. Commerce - Business, Finance & Accounting
    {
        Course comm("COMMERCE", "Commerce & Business Studies", "Financial accounting, business principles, corporate taxation");
        
        Subject finance("finance", "Financial Accounting & Valuation", "Balance sheets, cash flows, financial ratios");
        finance.addTopic(Topic("statements", "Financial Statements", "P&L, Cash Flow, Balance Sheet"));
        finance.addTopic(Topic("valuation", "Business Valuation", "DCF, multiples"));
        comm.addSubject(finance);

        courses.push_back(comm);
    }

    // 4. Common Track - General Aptitude, Reasoning, HR
    {
        Course common("COMMON", "General Placement & HR Track", "Quantitative Aptitude, Logical Reasoning, Behavioral HR");

        Subject aptitude("aptitude", "Quantitative Aptitude", "Numbers, percentages, time and work, probability");
        aptitude.addTopic(Topic("arithmetic", "Arithmetic", "Percentages, Profit/Loss, Time & Work"));
        aptitude.addTopic(Topic("probability", "Permutations & Probability", "Combinatorics, odds, probability"));
        common.addSubject(aptitude);

        Subject hr("hr", "HR & Behavioral Interview", "Leadership, conflict resolution, work culture, personal goals");
        hr.addTopic(Topic("behavioral", "Behavioral Scenarios", "STAR methodology, conflict handling"));
        hr.addTopic(Topic("career_vision", "Career Goals & Culture", "Self-introduction, long term aspirations"));
        common.addSubject(hr);

        courses.push_back(common);
    }

    return courses;
}
