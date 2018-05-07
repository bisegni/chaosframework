#ifndef CLINGLANGUAGESUPPORT_H
#define CLINGLANGUAGESUPPORT_H

#include "LanguageEditorSupport.h"

class CLINGLanguageSupport:
        public LanguageEditorSupport {
public:
    class Highlighter :
            public QSyntaxHighlighter {
    public:
        explicit Highlighter(QTextDocument *parent = 0);
        void highlightBlock(const QString& text);
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

public:
    CLINGLanguageSupport();
    ~CLINGLanguageSupport();

    const QString& getDescription();

    const QString& getVMPrefix();

    QString getCodeForHandler(LanguageHandler handler_type);

    QSyntaxHighlighter* getHiglighter(QTextDocument *parent);
};

#endif // CLINGLANGUAGESUPPORT_H
