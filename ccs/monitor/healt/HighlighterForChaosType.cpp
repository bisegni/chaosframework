#include "HighlighterForChaosType.h"

HighlighterForChaosType::HighlighterForChaosType(QTextDocument *parent):
QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    rule.pattern = QRegExp("[nt_[a-zA-Z]*_type]");
    rule.format.setForeground(Qt::darkGreen);
    rule.format.setFontWeight(QFont::Bold);
    highlightingRules.append(rule);
}

void HighlighterForChaosType::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
