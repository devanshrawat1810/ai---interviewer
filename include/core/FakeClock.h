#pragma once

#include "core/IClock.h"

// OOP Concept: Test Double (Mock / Fake) & Polymorphism
// Injectable clock with controllable date for deterministic unit testing.
class FakeClock : public IClock {
private:
    std::string currentDate_;

public:
    explicit FakeClock(std::string initialDate = "2026-10-09");
    ~FakeClock() override = default;

    std::string getCurrentDate() const override;
    void setDate(std::string date);
    void advanceDays(int days);
};
