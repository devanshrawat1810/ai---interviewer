#pragma once

#include <QFrame>

// OOP Concept: Container Component
// Elevated surface with clean borders and border radius.
class Card : public QFrame {
    Q_OBJECT

public:
    explicit Card(QWidget* parent = nullptr);
};
