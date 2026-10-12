#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <memory>
#include <vector>

class ServiceContext;
class Student;

// OOP Concept: Passive View in Model-View-Presenter
// Multi-scope leaderboard with rankings by Score, Solved Count, and Streak, and candidate row highlighting.
class LeaderboardView : public QWidget {
    Q_OBJECT

public:
    explicit LeaderboardView(std::shared_ptr<ServiceContext> ctx, QWidget* parent = nullptr);

    void refreshData(const QString& currentUserId);

signals:
    void backRequested();

private:
    std::shared_ptr<ServiceContext> ctx_;

    QComboBox* scopeCombo_;
    QComboBox* strategyCombo_;
    QTableWidget* table_;
    QLabel* stickyUserRankLabel_;

    void setupUi();
    void loadLeaderboard(const QString& currentUserId);
};
