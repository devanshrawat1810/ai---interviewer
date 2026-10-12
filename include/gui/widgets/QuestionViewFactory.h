#pragma once

#include <QWidget>
#include <memory>
#include "core/Question.h"

// OOP Concept: Abstract Factory / Polymorphic View Interface
// Base widget interface for questions rendered within the Interview screen.
class BaseQuestionWidget : public QWidget {
    Q_OBJECT

public:
    explicit BaseQuestionWidget(QWidget* parent = nullptr) : QWidget(parent) {}
    ~BaseQuestionWidget() override = default;

    virtual QString getAnswer() const = 0;
    virtual void clearAnswer() = 0;
};

// OOP Concept: Factory Pattern
// Instantiates the appropriate Question View (MCQ, Text, Coding) polymorphically.
class QuestionViewFactory {
public:
    static BaseQuestionWidget* createView(const std::shared_ptr<Question>& question, QWidget* parent = nullptr);
};
