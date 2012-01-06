/********************************************************************************
** Form generated from reading UI file 'uitoolkittest.ui'
**
** Created: Fri Jan 6 11:29:45 2012
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UITOOLKITTEST_H
#define UI_UITOOLKITTEST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UIToolkitTest
{
public:
    QAction *actionPreference;
    QWidget *centralWidget;
    QMdiArea *mdiArea;
    QWidget *subwindow;
    QGroupBox *groupBox;
    QPushButton *pushButton;
    QDoubleSpinBox *doubleSpinBox;
    QSpinBox *spinBox;
    QComboBox *comboBox;
    QMenuBar *menuBar;
    QMenu *menuMain;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *UIToolkitTest)
    {
        if (UIToolkitTest->objectName().isEmpty())
            UIToolkitTest->setObjectName(QString::fromUtf8("UIToolkitTest"));
        UIToolkitTest->resize(945, 710);
        actionPreference = new QAction(UIToolkitTest);
        actionPreference->setObjectName(QString::fromUtf8("actionPreference"));
        centralWidget = new QWidget(UIToolkitTest);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        mdiArea = new QMdiArea(centralWidget);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));
        mdiArea->setGeometry(QRect(30, 10, 781, 541));
        subwindow = new QWidget();
        subwindow->setObjectName(QString::fromUtf8("subwindow"));
        groupBox = new QGroupBox(subwindow);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(80, 50, 120, 80));
        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(-20, 30, 114, 32));
        doubleSpinBox = new QDoubleSpinBox(subwindow);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(270, 110, 62, 25));
        spinBox = new QSpinBox(subwindow);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(190, 180, 57, 25));
        comboBox = new QComboBox(subwindow);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(340, 30, 111, 26));
        mdiArea->addSubWindow(subwindow);
        UIToolkitTest->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(UIToolkitTest);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 945, 22));
        menuMain = new QMenu(menuBar);
        menuMain->setObjectName(QString::fromUtf8("menuMain"));
        UIToolkitTest->setMenuBar(menuBar);
        mainToolBar = new QToolBar(UIToolkitTest);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        UIToolkitTest->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(UIToolkitTest);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        UIToolkitTest->setStatusBar(statusBar);

        menuBar->addAction(menuMain->menuAction());
        menuMain->addAction(actionPreference);

        retranslateUi(UIToolkitTest);

        QMetaObject::connectSlotsByName(UIToolkitTest);
    } // setupUi

    void retranslateUi(QMainWindow *UIToolkitTest)
    {
        UIToolkitTest->setWindowTitle(QApplication::translate("UIToolkitTest", "UIToolkitTest", 0, QApplication::UnicodeUTF8));
        actionPreference->setText(QApplication::translate("UIToolkitTest", "Preference...", 0, QApplication::UnicodeUTF8));
        subwindow->setWindowTitle(QApplication::translate("UIToolkitTest", "Subwindow", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("UIToolkitTest", "GroupBox", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("UIToolkitTest", "PushButton", 0, QApplication::UnicodeUTF8));
        menuMain->setTitle(QApplication::translate("UIToolkitTest", "Main", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class UIToolkitTest: public Ui_UIToolkitTest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UITOOLKITTEST_H
