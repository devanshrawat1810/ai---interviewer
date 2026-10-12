#include "gui/widgets/CppSyntaxHighlighter.h"
#include <QColor>

CppSyntaxHighlighter::CppSyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {
    // Keywords (purple/indigo)
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor("#7C3AED"));
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        "\\bclass\\b", "\\bconst\\b", "\\benum\\b", "\\bexplicit\\b",
        "\\bfor\\b", "\\bif\\b", "\\belse\\b", "\\bnamespace\\b",
        "\\bnew\\b", "\\boperator\\b", "\\bprivate\\b", "\\bprotected\\b",
        "\\bpublic\\b", "\\breturn\\b", "\\bstruct\\b", "\\btemplate\\b",
        "\\btypename\\b", "\\bvirtual\\b", "\\bwhile\\b", "\\bauto\\b",
        "\\boverride\\b", "\\bnoexcept\\b", "\\bconstexpr\\b"
    };
    for (const QString& pattern : keywordPatterns) {
        rules_.append({QRegularExpression(pattern), keywordFormat});
    }

    // Primitive Types (blue)
    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor("#2563EB"));
    typeFormat.setFontWeight(QFont::Bold);
    const QString typePatterns[] = {
        "\\bint\\b", "\\bdouble\\b", "\\bfloat\\b", "\\bchar\\b",
        "\\bbool\\b", "\\bvoid\\b", "\\bstd::string\\b", "\\bstd::vector\\b"
    };
    for (const QString& pattern : typePatterns) {
        rules_.append({QRegularExpression(pattern), typeFormat});
    }

    // Strings (green)
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#16A34A"));
    rules_.append({QRegularExpression("\".*\""), stringFormat});

    // Single-line comments (slate grey)
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor("#64748B"));
    commentFormat.setFontItalic(true);
    rules_.append({QRegularExpression("//[^\n]*"), commentFormat});
}

void CppSyntaxHighlighter::highlightBlock(const QString& text) {
    for (const auto& rule : rules_) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
