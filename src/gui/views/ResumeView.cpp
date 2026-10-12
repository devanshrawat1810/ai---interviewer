#include "gui/views/ResumeView.h"
#include "gui/ServiceContext.h"
#include "gui/ThemeTokens.h"
#include "interview/ResumeInterviewMode.h"
#include <QtConcurrent>
#include <QFileDialog>

ResumeView::ResumeView(std::shared_ptr<ServiceContext> ctx, QWidget* parent)
    : QWidget(parent), ctx_(std::move(ctx)) {
    setupUi();
}

void ResumeView::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 24, 32, 24);
    rootLayout->setSpacing(16);

    auto* topBar = new QHBoxLayout();
    auto* backBtn = new QPushButton("← Back to Dashboard", this);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("background: transparent; border: none; font-weight: bold; color: #4F46E5;");
    connect(backBtn, &QPushButton::clicked, this, &ResumeView::backRequested);

    auto* titleLabel = new QLabel("Resume-Based Mock Interview Setup", this);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #0F172A;");

    topBar->addWidget(backBtn);
    topBar->addSpacing(16);
    topBar->addWidget(titleLabel);
    topBar->addStretch();
    rootLayout->addLayout(topBar);

    // Privacy Consent Banner
    auto* consentCard = new Card(this);
    consentCard->setStyleSheet("background: #FEF3C7; border: 1px solid #FDE68A;");
    auto* cLay = new QVBoxLayout(consentCard);
    auto* cText = new QLabel(QString::fromStdString(ResumeInterviewMode::getPrivacyConsentNotice()), consentCard);
    cText->setWordWrap(true);
    cText->setStyleSheet("color: #92400E; font-size: 13px; line-height: 1.4;");

    consentBox_ = new QCheckBox("I understand and consent to processing my resume text for tailored question generation.", consentCard);
    consentBox_->setStyleSheet("font-weight: bold; color: #78350F;");

    cLay->addWidget(cText);
    cLay->addWidget(consentBox_);
    rootLayout->addWidget(consentCard);

    // Resume Editor & File Picker
    auto* edCard = new Card(this);
    auto* edLayout = new QVBoxLayout(edCard);

    auto* edBar = new QHBoxLayout();
    auto* edTitle = new QLabel("Paste Resume Text (Max 6,000 Characters):", edCard);
    edTitle->setStyleSheet("font-weight: bold; color: #1E293B;");

    auto* openFileBtn = new QPushButton("Open .txt File...", edCard);
    openFileBtn->setCursor(Qt::PointingHandCursor);
    openFileBtn->setStyleSheet("background: transparent; border: 1px solid #CBD5E1; border-radius: 6px; padding: 4px 10px; font-size: 12px;");

    connect(openFileBtn, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "Select Resume File", "", "Text Files (*.txt)");
        if (!path.isEmpty()) {
            std::string err;
            std::string text = ResumeInterviewMode::loadResumeFile(path.toStdString(), err);
            if (!text.empty()) {
                resumeEditor_->setPlainText(QString::fromStdString(text));
            }
        }
    });

    edBar->addWidget(edTitle);
    edBar->addStretch();
    edBar->addWidget(openFileBtn);

    resumeEditor_ = new QTextEdit(edCard);
    resumeEditor_->setMinimumHeight(140);
    resumeEditor_->setPlaceholderText("Paste your technical experience, projects, languages, and tools here...");

    charCounterLabel_ = new QLabel("0 / 6000 characters", edCard);
    charCounterLabel_->setAlignment(Qt::AlignRight);
    charCounterLabel_->setStyleSheet("color: #64748B; font-size: 12px;");

    connect(resumeEditor_, &QTextEdit::textChanged, this, [this]() {
        int len = resumeEditor_->toPlainText().length();
        charCounterLabel_->setText(QString("%1 / 6000 characters").arg(len));
        if (len > 6000) {
            charCounterLabel_->setStyleSheet("color: #DC2626; font-size: 12px; font-weight: bold;");
        } else {
            charCounterLabel_->setStyleSheet("color: #64748B; font-size: 12px;");
        }
    });

    errorLabel_ = new QLabel(edCard);
    errorLabel_->setStyleSheet("color: #DC2626; font-size: 13px; font-weight: bold;");
    errorLabel_->setVisible(false);

    analyzeBtn_ = new PrimaryButton("Analyze Resume & Generate Questions", edCard);
    analyzeBtn_->setFixedWidth(280);

    edLayout->addLayout(edBar);
    edLayout->addWidget(resumeEditor_);
    edLayout->addWidget(charCounterLabel_);
    edLayout->addWidget(errorLabel_);
    edLayout->addWidget(analyzeBtn_);
    rootLayout->addWidget(edCard);

    // Preview Card (Shown after analysis)
    previewCard_ = new Card(this);
    auto* pLay = new QVBoxLayout(previewCard_);
    auto* pTitle = new QLabel("Extracted Candidate Profile & Skills Confirmation", previewCard_);
    pTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1E293B;");

    summaryLabel_ = new QLabel(previewCard_);
    summaryLabel_->setWordWrap(true);
    summaryLabel_->setStyleSheet("color: #334155; font-size: 13px;");

    skillsLabel_ = new QLabel(previewCard_);
    skillsLabel_->setWordWrap(true);
    skillsLabel_->setStyleSheet("font-weight: 600; color: #4F46E5; font-size: 13px;");

    launchBtn_ = new PrimaryButton("Confirm & Launch Resume Interview", previewCard_);
    launchBtn_->setFixedWidth(280);

    pLay->addWidget(pTitle);
    pLay->addWidget(summaryLabel_);
    pLay->addWidget(skillsLabel_);
    pLay->addWidget(launchBtn_);
    previewCard_->setVisible(false);
    rootLayout->addWidget(previewCard_);

    // Connect Analysis Action
    connect(analyzeBtn_, &QPushButton::clicked, this, [this]() {
        if (!consentBox_->isChecked()) {
            errorLabel_->setText("You must accept the privacy consent before proceeding.");
            errorLabel_->setVisible(true);
            return;
        }

        QString text = resumeEditor_->toPlainText().trimmed();
        std::string err;
        if (!ResumeInterviewMode::validateResumeLength(text.toStdString(), err)) {
            errorLabel_->setText(QString::fromStdString(err));
            errorLabel_->setVisible(true);
            return;
        }

        errorLabel_->setVisible(false);
        analyzeBtn_->setLoading(true);

        (void)QtConcurrent::run([this, text]() {
            auto res = ctx_->resumeAnalyzer->analyzeResume(text.toStdString(), "CSE");
            QMetaObject::invokeMethod(this, [this, res]() {
                analyzeBtn_->setLoading(false);
                if (res.success) {
                    summaryLabel_->setText(QString("<b>Experience Summary:</b> %1").arg(QString::fromStdString(res.experienceSummary)));
                    QString sList;
                    for (const auto& s : res.extractedSkills) sList += QString::fromStdString(s) + ", ";
                    skillsLabel_->setText(QString("<b>Confirmed Skills:</b> %1").arg(sList));
                    previewCard_->setVisible(true);

                    // Connect final launch
                    disconnect(launchBtn_, nullptr, nullptr, nullptr);
                    connect(launchBtn_, &QPushButton::clicked, this, [this, res]() {
                        ResumeInterviewMode mode;
                        auto session = mode.createResumeSession("user_demo_1", res);
                        emit resumeSessionReady(std::shared_ptr<InterviewSession>(std::move(session)));
                    });
                } else {
                    errorLabel_->setText("Analysis failed: " + QString::fromStdString(res.errorMessage));
                    errorLabel_->setVisible(true);
                }
            }, Qt::QueuedConnection);
        });
    });

    rootLayout->addStretch();
}
