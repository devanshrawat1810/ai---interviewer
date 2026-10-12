#include <QTest>
#include <QSignalSpy>
#include "gui/Result.h"
#include "gui/ErrorTranslator.h"
#include "gui/ThemeTokens.h"
#include "gui/ThemeManager.h"
#include "gui/widgets/QuestionViewFactory.h"
#include "gui/widgets/MCQQuestionWidget.h"
#include "gui/widgets/TextQuestionWidget.h"
#include "gui/widgets/CodingQuestionWidget.h"
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include "core/CodingQuestion.h"
#include "admin/ValidationException.h"
#include "db/DatabaseExceptions.h"

// OOP Concept: Unit Testing Presentation Components with Qt Test
// Headless test suite verifying Result<T>, ErrorTranslator, ThemeManager, and QuestionViewFactory.
class TestGuiPresenters : public QObject {
    Q_OBJECT

private slots:
    void testResultSuccessAndError();
    void testErrorTranslatorExceptions();
    void testErrorTranslatorPlainStrings();
    void testThemeManagerToggle();
    void testQuestionViewFactoryPolymorphic();
};

void TestGuiPresenters::testResultSuccessAndError() {
    auto okRes = Result<int>::ok(42);
    QVERIFY(okRes.isOk());
    QVERIFY(!okRes.isError());
    QCOMPARE(okRes.value(), 42);

    auto errRes = Result<std::string>::error("Network timeout");
    QVERIFY(errRes.isError());
    QVERIFY(!errRes.isOk());
    QCOMPARE(QString::fromStdString(errRes.error()), QString("Network timeout"));

    auto voidOk = Result<void>::ok();
    QVERIFY(voidOk.isOk());
}

void TestGuiPresenters::testErrorTranslatorExceptions() {
    ConnectionException connEx("Failed to connect to Mongo server");
    std::string userMsg1 = ErrorTranslator::translate(connEx);
    QVERIFY(QString::fromStdString(userMsg1).contains("database"));

    DuplicateKeyException dupEx("Duplicate email in users collection");
    std::string userMsg2 = ErrorTranslator::translate(dupEx);
    QVERIFY(QString::fromStdString(userMsg2).contains("already exists"));

    ValidationException valEx("Question text cannot be blank");
    std::string userMsg3 = ErrorTranslator::translate(valEx);
    QVERIFY(QString::fromStdString(userMsg3).contains("Question text cannot be blank"));
}

void TestGuiPresenters::testErrorTranslatorPlainStrings() {
    std::string lockout = ErrorTranslator::translate("Account locked after failed logins");
    QVERIFY(QString::fromStdString(lockout).contains("15 minutes"));

    std::string pwd = ErrorTranslator::translate("Invalid password provided");
    QVERIFY(QString::fromStdString(pwd).contains("credentials"));
}

void TestGuiPresenters::testThemeManagerToggle() {
    auto& tm = ThemeManager::instance();
    tm.setTheme(ThemeManager::ThemeMode::Light);
    QVERIFY(!tm.isDarkMode());

    tm.setTheme(ThemeManager::ThemeMode::Dark);
    QVERIFY(tm.isDarkMode());

    QString qss = tm.generateStyleSheet();
    QVERIFY(!qss.isEmpty());
    QVERIFY(qss.contains("background-color"));
}

void TestGuiPresenters::testQuestionViewFactoryPolymorphic() {
    // MCQ Question
    auto mcq = std::make_shared<MCQQuestion>(
        "q_mcq", "OOP", "Polymorphism", Difficulty::EASY,
        "What keyword enables runtime polymorphism in C++?",
        "virtual", std::vector<std::string>{"virtual", "static", "friend"}, "A"
    );
    BaseQuestionWidget* mcqView = QuestionViewFactory::createView(mcq, nullptr);
    QVERIFY(dynamic_cast<MCQQuestionWidget*>(mcqView) != nullptr);
    delete mcqView;

    // Text Question
    auto textQ = std::make_shared<TextQuestion>(
        "q_txt", "OOP", "Design", Difficulty::MEDIUM,
        "Explain Encapsulation in C++.", "Hiding data",
        "Encapsulation is bundling data and methods.",
        std::vector<std::string>{"private", "class", "getter"}
    );
    BaseQuestionWidget* textView = QuestionViewFactory::createView(textQ, nullptr);
    QVERIFY(dynamic_cast<TextQuestionWidget*>(textView) != nullptr);
    delete textView;

    // Coding Question
    auto codingQ = std::make_shared<CodingQuestion>(
        "q_code", "CPP", "STL", Difficulty::HARD,
        "Implement Stack", "class Stack",
        "#include <iostream>\nint main() { return 0; }",
        std::vector<TestCase>{}, 20
    );
    BaseQuestionWidget* codeView = QuestionViewFactory::createView(codingQ, nullptr);
    QVERIFY(dynamic_cast<CodingQuestionWidget*>(codeView) != nullptr);
    delete codeView;
}

QTEST_MAIN(TestGuiPresenters)
#include "test_gui_presenters.moc"
