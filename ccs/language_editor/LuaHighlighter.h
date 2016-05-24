#ifndef LUAHIGHLIGHTER_H
#define LUAHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class LuaHighlighter :
        public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit LuaHighlighter(QTextDocument *parent = 0);
    void highlightBlock(const QString& text);

signals:

public slots:

private:

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat umlaut;
    QTextCharFormat numbers;

    QColor clrUmlauts;
    QColor clrNumbers;
    QColor clrSingleComment;
    QColor clrMultiComment;
    QColor clrDoubleQuote;
    QColor clrSingeleQuote;
    QColor clrFunction;

};

#endif // LUAHIGHLIGHTER_H
