#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "feedback/FeedbackGenerator.h"
#include "gui/widgets/Card.h"
#include "gui/widgets/ChartWidget.h"
#include "gui/widgets/PrimaryButton.h"

// OOP Concept: Passive View in Model-View-Presenter
// Comprehensive feedback analysis screen with score hero, custom difficulty bar charts, and suggestions.
class FeedbackView : public QWidget {
    Q_OBJECT

public:
    explicit FeedbackView(QWidget* parent = nullptr);

    void setReport(const FeedbackReport& report);

signals:
    void returnToDashboardClicked();
    void retryInterviewClicked();

private:
    QLabel* scoreHeroLabel_;
    QLabel* percentLabel_;
    QLabel* noticeBanner_;
    QLabel* aiSummaryLabel_;
    QLabel* strongestTopicLabel_;
    QLabel* weakestTopicLabel_;

    ChartWidget* chartWidget_;
    QVBoxLayout* suggestionsLayout_;

    PrimaryButton* returnBtn_;
    QPushButton* retryBtn_;

    void setupUi();
};
