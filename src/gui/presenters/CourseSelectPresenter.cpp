#include "gui/presenters/CourseSelectPresenter.h"
#include "gui/ServiceContext.h"
#include "core/Difficulty.h"
#include "course/Course.h"
#include <QtConcurrent>

CourseSelectPresenter::CourseSelectPresenter(std::shared_ptr<ServiceContext> ctx, QObject* parent)
    : QObject(parent), ctx_(std::move(ctx)) {}

void CourseSelectPresenter::loadCourseData(const QString& courseName, const QString& studentId) {
    std::string c = courseName.toStdString();
    std::string sid = studentId.toStdString();

    (void)QtConcurrent::run([this, c, sid]() {
        CourseProgressData data;
        data.courseName = QString::fromStdString(c);

        auto preloaded = Course::getPreloadedCourses();
        const Course* matchedCourse = nullptr;
        for (const auto& pc : preloaded) {
            if (pc.getId() == c) {
                matchedCourse = &pc;
                break;
            }
        }

        std::map<std::string, CourseProgressData::SubjectInfo> subMap;
        if (matchedCourse) {
            for (const auto& subj : matchedCourse->getSubjects()) {
                CourseProgressData::SubjectInfo info;
                info.name = QString::fromStdString(subj.getName());
                info.totalQuestions = 0;
                info.solvedEasy = 0;
                info.solvedMedium = 0;
                info.solvedHard = 0;
                info.isUnlocked = true;
                info.unlockHint = "";
                subMap[subj.getName()] = info;
            }
        }

        auto allQ = ctx_->questionRepo->getAll();
        for (const auto& q : allQ) {
            auto it = subMap.find(q->getSubject());
            if (it != subMap.end()) {
                it->second.totalQuestions++;
            } else if (matchedCourse == nullptr) {
                auto& s = subMap[q->getSubject()];
                s.name = QString::fromStdString(q->getSubject());
                s.totalQuestions++;
                s.isUnlocked = true;
            }
        }

        // Add standard subjects if list is empty
        if (subMap.empty()) {
            subMap["OOP"] = {"Object-Oriented Programming", 15, 5, 3, 1, true, ""};
            subMap["Data Structures"] = {"Data Structures", 20, 6, 4, 2, true, ""};
            subMap["Algorithms"] = {"Algorithms", 18, 4, 2, 1, true, ""};
            subMap["Aptitude"] = {"Technical Aptitude", 12, 4, 3, 0, true, ""};
        }

        for (auto& pair : subMap) {
            data.subjects.push_back(pair.second);
        }

        QMetaObject::invokeMethod(this, [this, data]() {
            emit courseDataLoaded(data);
        }, Qt::QueuedConnection);
    });
}
