#ifndef MESSAGEUTILITY_H
#define MESSAGEUTILITY_H

#include <QString>

class MessageUtility {
public:
    static bool showYNDialog(const QString& text,
                             const QString& information_text,
                             bool default_button = false);
};

#endif // MESSAGEUTILITY_H
