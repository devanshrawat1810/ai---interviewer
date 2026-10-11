#pragma once

#include "feedback/FeedbackGenerator.h"

// OOP Concept: Polymorphism & Inheritance
// Concrete feedback generator analyzing session metrics deterministically:
// - Aggregates points per difficulty level (Easy, Medium, Hard).
// - Identifies strongest and weakest topics based on percentage accuracy.
// - Formulates actionable targeted study suggestions.
class RuleBasedFeedback : public FeedbackGenerator {
public:
    RuleBasedFeedback() = default;
    ~RuleBasedFeedback() override = default;

    FeedbackReport generateFeedback(const InterviewSession& session) override;
};
