/********************************************************************************
** Form generated from reading UI file 'controldialog.ui'
**
** Created: Mon Feb 13 18:57:59 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLDIALOG_H
#define UI_CONTROLDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonCommit;
    QPushButton *buttonClose;

    void setupUi(QDialog *ControlDialog)
    {
        if (ControlDialog->objectName().isEmpty())
            ControlDialog->setObjectName(QString::fromUtf8("ControlDialog"));
        ControlDialog->resize(491, 279);
        verticalLayout = new QVBoxLayout(ControlDialog);
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        label = new QLabel(ControlDialog);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        widget = new QWidget(ControlDialog);
        widget->setObjectName(QString::fromUtf8("widget"));

        verticalLayout->addWidget(widget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonCommit = new QPushButton(ControlDialog);
        buttonCommit->setObjectName(QString::fromUtf8("buttonCommit"));

        horizontalLayout->addWidget(buttonCommit);

        buttonClose = new QPushButton(ControlDialog);
        buttonClose->setObjectName(QString::fromUtf8("buttonClose"));

        horizontalLayout->addWidget(buttonClose);


        verticalLayout->addLayout(horizontalLayout);

        verticalLayout->setStretch(1, 1);

        retranslateUi(ControlDialog);

        QMetaObject::connectSlotsByName(ControlDialog);
    } // setupUi

    void retranslateUi(QDialog *ControlDialog)
    {
        ControlDialog->setWindowTitle(QApplication::translate("ControlDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ControlDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        buttonCommit->setText(QApplication::translate("ControlDialog", "Commit", 0, QApplication::UnicodeUTF8));
        buttonClose->setText(QApplication::translate("ControlDialog", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ControlDialog: public Ui_ControlDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLDIALOG_H
