#include "CLINGLanguageSupport.h"

#pragma mark Highlighter

CLINGLanguageSupport::Highlighter::Highlighter(QTextDocument *parent):
    QSyntaxHighlighter(parent) {
    QStringList keywordPatterns;
    HighlightingRule rule;

    keywordFormat.setForeground(QColor(187,125,215));
    keywordFormat.setFontWeight(QFont::Bold);

    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                        << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                        << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                        << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                        << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                        << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                        << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                        << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                        << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                        << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b";
    foreach (const QString &pattern, keywordPatterns)     {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor(115,174,233));
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}


void CLINGLanguageSupport::Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);


    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);


    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

#pragma mark CLINGLanguageSupport
CLINGLanguageSupport::CLINGLanguageSupport():
    LanguageEditorSupport("C++ [CLING]", "CPP"){}

CLINGLanguageSupport::~CLINGLanguageSupport() {}

QString CLINGLanguageSupport::getCodeForHandler(LanguageHandler handler_type) {
    QString result;
    switch(handler_type) {
    case ChaosWrapper:
        result = "";
        break;
    case LaunchHandler:
        result = "extern \"C\" int algorithmLaunch(const std::vector<chaos::common::data::CDataVariant>& in_args, std::vector<chaos::common::data::CDataVariant>& out_args){return 0;}";
        break;
    case StartHandler:
        result = "extern \"C\" int algorithmStart(const std::vector<chaos::common::data::CDataVariant>& in_args, std::vector<chaos::common::data::CDataVariant>& out_args){return 0;}";
        break;
    case  StepHandler:
        result = "extern \"C\" int algorithmStep(const std::vector<chaos::common::data::CDataVariant>& in_args, std::vector<chaos::common::data::CDataVariant>& out_args){return 0;}";
        break;
    case StopHandler:
        result = "extern \"C\" int algorithmStop(const std::vector<chaos::common::data::CDataVariant>& in_args, std::vector<chaos::common::data::CDataVariant>& out_args){return 0;}";
        break;
    case TerminateHandler:
        result = "extern \"C\" int algorithmEnd(const std::vector<chaos::common::data::CDataVariant>& in_args, std::vector<chaos::common::data::CDataVariant>& out_args){return 0;}";
        break;
    case AttributeChangedHHandler:
        result = "extern \"C\" int inputAttributeChanged(const std::vector<chaos::common::data::CDataVariant>& in_args, std::vector<chaos::common::data::CDataVariant>& out_args){return 0;}";
        break;
    }
    return result;
}

QSyntaxHighlighter* CLINGLanguageSupport::getHiglighter(QTextDocument *parent) {
    return new CLINGLanguageSupport::Highlighter(parent);
}

