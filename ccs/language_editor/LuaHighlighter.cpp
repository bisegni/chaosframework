
#include "LuaHighlighter.h"
#include <cstdio>

LuaHighlighter::LuaHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent) {
    endColor.setRgb(0x00, 0x00, 0xFF);
    commentColor.setRgb(0x08, 0xA0, 0x80);
}

void LuaHighlighter::highlightBlock(const QString& text)
{
    QFont bold;
    bold.setBold(true);

    for(int i = 0; i < text.length(); i++) {
        if(text.at(i).isNumber()) {
            setFormat(i, 1, QColor(0xFF, 0x80, 0x00));
        }

        QString edited = text;
        edited = edited.replace("\t", " ");

        QStringList list = edited.split(" ");
        QString word;

        for(int i = 0; i < list.size(); i++) {
            word = list.at(i);
            if(word == "end")
                setFormat(text.indexOf(word), word.length(), endColor);
            else if(word == "function")
                setFormat(text.indexOf(word), word.length(), endColor);
            else if(word == "if" || word == "else" || word == "then" || word == "elseif" || word == "nil" || word == "for" || word == "do")
                setFormat(text.indexOf(word), word.length(), bold);
        }

        int idx;
        // Comments
        idx = text.indexOf("--");
        int comment = idx;

        if(idx != -1) {
            setFormat(idx, text.length() - idx, commentColor);
        }

        // Quotes
        idx = text.indexOf("\"");

        if(idx != -1 && (idx < comment || comment == -1)) {
            int idx2 = text.indexOf("\"", idx + 1);

            if(idx2 != -1) {
                idx2++;
                setFormat(idx, idx2 - idx, Qt::red);
            }
        }
    }
}
