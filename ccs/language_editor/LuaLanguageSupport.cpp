#include "LuaLanguageSupport.h"

#pragma mark LuaLanguageSupport::LuaHighlighter

LuaLanguageSupport::Highlighter::Highlighter(QTextDocument *parent):
    QSyntaxHighlighter(parent) {
    QStringList keywordPatterns;
    HighlightingRule rule;

    keywordFormat.setForeground(QColor(187,125,215));
    keywordFormat.setFontWeight(QFont::Bold);

    keywordPatterns << "\\bfunction\\b"     << "\\bend\\b"          << "\\bwhile\\b"
                    << "\\bfor\\b"          << "\\buntil\\b"        << "\\band\\b"
                    << "\\bbreak\\b"        << "\\bdo\\b"           << "\\belse\\b"
                    << "\\bbelseif\\b"      << "\\bfalse\\b"        << "\\bif\\b"
                    << "\\bin\\b"           << "\\blocal\\b"        << "\\bnil\\b"
                    << "\\bnot\\b"          << "\\bor\\b"           << "\\breturn\\b"
                    << "\\brepeat\\b"       << "\\bthen\\b"         << "\\btrue\\b"
                    << "\\btonumber\\b"     << "\\btostring\\b"     << "\\bprint\\b"
                    << "\\berror\\b"        << "\\bipairs\\b"       << "\\bpairs\\b"
                    << "\\bsetmetatable\\b" << "\\bgetmetatable\\b" << "\\btype\\b"
                    << "\\bselect\\b"       << "\\bloadstring\\b"   << "\\bloadfile\\b"
                    << "\\barg\\b"          << "\\bmath\\b"         << "\\bstring\\b"
                    << "\\bos\\b"           << "\\bio\\b"           << "\\belseif\\b";
    foreach (const QString &pattern, keywordPatterns)     {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    clrUmlauts = Qt::red;
    clrNumbers = Qt::magenta;
    clrSingleComment = Qt::lightGray;
    clrMultiComment = Qt::lightGray;
    clrDoubleQuote = QColor(161,193,127);
    clrSingeleQuote = QColor(161,193,127);
    clrFunction = QColor(115,174,233);


    //These characters aren't good in lua
    umlaut.setFontWeight(QFont::Bold);
    umlaut.setForeground(clrUmlauts);
    rule.pattern = QRegExp("[ÃÃÃÃÃ¤Ã¶Ã¼]");
    rule.format = umlaut;
    highlightingRules.append(rule);


    //0 1 2 3 4 5 6 7 8 9
    numbers.setForeground(clrNumbers);
    rule.pattern = QRegExp("[0-9]");
    rule.format = numbers;
    highlightingRules.append(rule);


    // Single line comment -> --
    singleLineCommentFormat.setForeground(clrSingleComment);
    rule.pattern = QRegExp("--+[^[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(clrMultiComment);


    //"Quotation"
    quotationFormat.setForeground(clrDoubleQuote);
    rule.pattern = QRegExp("\".*\"");
    rule.pattern = QRegExp( "(?:^|[^\\\\'])(\"(?:\\\\\"|\\\\(?!\")|[^\\\\\"^Ã¤^Ã¶^Ã¼])*\")" );
    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule);


    //'Quotation'
    quotationFormat.setForeground(clrSingeleQuote);
    rule.pattern = QRegExp("\'.*\'");
    rule.pattern = QRegExp( "(?:^|[^\\\\\"])(\'(?:\\\\\'|\\\\(?!\')|[^\\\\\'])*\')" );
    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule);



    //function name()
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(clrFunction);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);


    //Multi Line Comment --[[ ]]
    commentStartExpression = QRegExp("--\\[\\["); // --[[
    commentEndExpression = QRegExp("\\]\\]"); // ]]

    rule.pattern.setMinimal(false);
}


void LuaLanguageSupport::Highlighter::highlightBlock(const QString &text)
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

#pragma mark LuaLanguageSupport
LuaLanguageSupport::LuaLanguageSupport():
LanguageEditorSupport("Lua", "Lua"){}

LuaLanguageSupport::~LuaLanguageSupport() {}

QString LuaLanguageSupport::getCodeForHandler(LanguageHandler handler_type) {
    QString result;
    switch(handler_type) {
    case ChaosWrapper:
        result = "local chaos = chaos()";
        break;
    case LaunchHandler:
        result = "function algorithmLaunch()\n\tprint ( \"executing algorithmLaunch\" );\nend";
        break;
    case StartHandler:
        result = "function algorithmStart()\n\tprint ( \"executing algorithmStart\" );\nend";
        break;
    case  StepHandler:
        result = "function algorithmStep(delay_from_last_step) \n\tprint ( \"executing algorithmStep--->\"..tostring(delay_from_last_step) );\nend";
        break;
    case StopHandler:
        result = "function algorithmStop()\n\tprint ( \"executing algorithmStop\" );\nend";
        break;
    case TerminateHandler:
        result = "function algorithmEnd()\n\tprint ( \"executing algorithmEnd\" );\nend";
        break;
    case AttributeChangedHHandler:
        result = "function inputAttributeChanged(attribute_name)\n\tprint ( \"signal received for input dataset update\" );\nend";
        break;
    }
    return result;
}

QSyntaxHighlighter* LuaLanguageSupport::getHiglighter(QTextDocument *parent) {
    return new LuaLanguageSupport::Highlighter(parent);
}
