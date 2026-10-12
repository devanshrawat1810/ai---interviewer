#pragma once

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <memory>
#include "gui/widgets/Card.h"
#include "gui/widgets/PrimaryButton.h"

class ServiceContext;
class InterviewSession;

// OOP Concept: Passive View in Model-View-Presenter & Privacy Safeguards
// Resume upload workflow with privacy consent, live character counting, and extracted skills review.
class ResumeView : public QWidget {
    Q_OBJECT

public:
    explicit ResumeView(std::shared_ptr<ServiceContext> ctx, QWidget* parent = nullptr);

signals:
    void backRequested();
    void resumeSessionReady(std::shared_ptr<InterviewSession> session);

private:
    std::shared_ptr<ServiceContext> ctx_;

    QCheckBox* consentBox_;
    QTextEdit* resumeEditor_;
    QLabel* charCounterLabel_;
    QLabel* errorLabel_;
    PrimaryButton* analyzeBtn_;

    Card* previewCard_;
    QLabel* summaryLabel_;
    QLabel* skillsLabel_;
    PrimaryButton* launchBtn_;

    void setupUi();
};
