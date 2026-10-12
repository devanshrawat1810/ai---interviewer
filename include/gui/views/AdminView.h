#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <memory>

class ServiceContext;
class Admin;

// OOP Concept: Passive View in Model-View-Presenter & Role-Based Access Control
// Admin panel managing curriculum questions, validation rules, soft deletes, and system stats.
class AdminView : public QWidget {
    Q_OBJECT

public:
    explicit AdminView(std::shared_ptr<ServiceContext> ctx, QWidget* parent = nullptr);

    void refreshAdminData(std::shared_ptr<Admin> admin);

signals:
    void backRequested();

private:
    std::shared_ptr<ServiceContext> ctx_;
    std::shared_ptr<Admin> currentAdmin_;

    QLabel* statsLabel_;
    QTableWidget* questionsTable_;
    QPushButton* softDeleteBtn_;

    void setupUi();
    void loadQuestions();
};
