#pragma once

#include <QObject>
#include <memory>
#include <QString>
#include <vector>

class ServiceContext;
class Question;

struct CourseProgressData {
    QString courseName;
    struct SubjectInfo {
        QString name;
        int totalQuestions{0};
        int solvedEasy{0};
        int solvedMedium{0};
        int solvedHard{0};
        bool isUnlocked{true};
        QString unlockHint;
    };
    std::vector<SubjectInfo> subjects;
};

// OOP Concept: Presenter in Model-View-Presenter
// Computes subject-level question statistics, unlocked tracks, and interview mode selection.
class CourseSelectPresenter : public QObject {
    Q_OBJECT

public:
    explicit CourseSelectPresenter(std::shared_ptr<ServiceContext> ctx, QObject* parent = nullptr);

    void loadCourseData(const QString& courseName, const QString& studentId);

signals:
    void courseDataLoaded(const CourseProgressData& data);

private:
    std::shared_ptr<ServiceContext> ctx_;
};
