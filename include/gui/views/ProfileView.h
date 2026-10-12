#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <memory>
#include <vector>
#include <string>
#include "gui/widgets/Card.h"
#include "gui/widgets/FormField.h"
#include "gui/widgets/PrimaryButton.h"

class Student;

// OOP Concept: Passive View in Model-View-Presenter
// Candidate profile management, monthly streak visualization grid, and achievement badges.
class ProfileView : public QWidget {
    Q_OBJECT

public:
    explicit ProfileView(QWidget* parent = nullptr);

    void setStudentData(std::shared_ptr<Student> student);

signals:
    void backRequested();
    void profileUpdated(const QString& college, const QString& branch);

private:
    std::shared_ptr<Student> currentStudent_;

    QLabel* avatarLabel_;
    QLabel* nameLabel_;
    QLabel* emailLabel_;

    FormField* collegeField_;
    FormField* branchField_;
    PrimaryButton* saveBtn_;

    QGridLayout* calendarGrid_;
    QVBoxLayout* badgesLayout_;

    void setupUi();
    void populateCalendar(const std::vector<std::string>& activeDates);
};
