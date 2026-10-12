#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "gui/presenters/CourseSelectPresenter.h"
#include "gui/widgets/Card.h"

// OOP Concept: Passive View in Model-View-Presenter
// Course subject browser with progress bars, locked status icons, and interview mode selection.
class CourseSelectView : public QWidget {
    Q_OBJECT

public:
    explicit CourseSelectView(CourseSelectPresenter* presenter, QWidget* parent = nullptr);

public slots:
    void updateCourseData(const CourseProgressData& data);

signals:
    void subjectSelected(const QString& subject, const QString& mode);
    void backRequested();

private:
    CourseSelectPresenter* presenter_;
    QLabel* breadcrumbLabel_;
    QVBoxLayout* subjectsLayout_;
    void setupUi();
};
