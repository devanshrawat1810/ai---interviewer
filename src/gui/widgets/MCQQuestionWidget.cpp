#include "gui/widgets/MCQQuestionWidget.h"
#include <QKeySequence>
#include <QShortcut>

MCQQuestionWidget::MCQQuestionWidget(const std::vector<std::string>& options, QWidget* parent)
    : BaseQuestionWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    group_ = new QButtonGroup(this);

    const char labels[] = {'A', 'B', 'C', 'D', 'E', 'F'};

    for (size_t i = 0; i < options.size(); ++i) {
        char prefix = (i < sizeof(labels)) ? labels[i] : static_cast<char>('A' + i);
        QString optText = QString("%1. %2").arg(prefix).arg(QString::fromStdString(options[i]));

        auto* radio = new QRadioButton(optText, this);
        radio->setCursor(Qt::PointingHandCursor);
        radio->setStyleSheet(R"(
            QRadioButton {
                padding: 12px 16px;
                border: 1px solid #E2E8F0;
                border-radius: 8px;
                background-color: #FFFFFF;
                font-size: 14px;
            }
            QRadioButton:hover {
                border-color: #4F46E5;
                background-color: #F8FAFC;
            }
            QRadioButton:checked {
                border: 2px solid #4F46E5;
                background-color: #EEF2FF;
                font-weight: bold;
            }
        )");

        group_->addButton(radio, static_cast<int>(i));
        radioButtons_.push_back(radio);
        layout->addWidget(radio);

        // Keyboard shortcuts (1-4 and A-D)
        auto* keyShortcut = new QShortcut(QKeySequence(QString(prefix)), this);
        connect(keyShortcut, &QShortcut::activated, radio, [radio]() { radio->setChecked(true); });
        auto* numShortcut = new QShortcut(QKeySequence(QString::number(i + 1)), this);
        connect(numShortcut, &QShortcut::activated, radio, [radio]() { radio->setChecked(true); });
    }
}

QString MCQQuestionWidget::getAnswer() const {
    int id = group_->checkedId();
    if (id >= 0 && static_cast<size_t>(id) < radioButtons_.size()) {
        char prefix = static_cast<char>('A' + id);
        return QString(prefix);
    }
    return "";
}

void MCQQuestionWidget::clearAnswer() {
    auto* checked = group_->checkedButton();
    if (checked) {
        group_->setExclusive(false);
        checked->setChecked(false);
        group_->setExclusive(true);
    }
}
