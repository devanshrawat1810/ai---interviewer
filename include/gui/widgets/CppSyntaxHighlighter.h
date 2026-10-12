#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QVector>

// OOP Concept: Specialization & Lexical Analysis
// Highlights C++17 keywords, types, strings, comments, and preprocessor directives.
class CppSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit CppSyntaxHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> rules_;
};
