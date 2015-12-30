#ifndef HIGHLIGHTERFORCHAOSTYPE_H
#define HIGHLIGHTERFORCHAOSTYPE_H

#include <QSyntaxHighlighter>

class HighlighterForChaosType:
        public QSyntaxHighlighter {
    Q_OBJECT
protected:
    void highlightBlock(const QString &text);
public:
    HighlighterForChaosType(QTextDocument *parent = 0);

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
};

#endif // HIGHLIGHTERFORCHAOSTYPE_H
