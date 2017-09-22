#include "MessageUtility.h"

#include <QMessageBox>

bool MessageUtility::showYNDialog(const QString& text,
                                  const QString& information_text,
                                  bool default_button) {
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.setInformativeText(information_text);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(default_button?QMessageBox::Yes:QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}

