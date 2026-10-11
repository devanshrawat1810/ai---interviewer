#pragma once

#include "core/IClock.h"

// OOP Concept: Interface Implementation & Polymorphism
// Production clock reading system time from standard library chrono.
class SystemClock : public IClock {
public:
    SystemClock() = default;
    ~SystemClock() override = default;

    std::string getCurrentDate() const override;
};
