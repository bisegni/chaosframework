/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun Feb 12 20:13:24 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDial>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QSplitter>
#include <QtGui/QTableView>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionShow_Info;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QSplitter *splitter;
    QWidget *layoutWidget1;
    QVBoxLayout *rightVerticalLayout;
    QLabel *label;
    QListView *listView;
    QHBoxLayout *commandButtonHorizontalLayout;
    QPushButton *buttonDeleteDevice;
    QPushButton *buttonUpdateDeviceList;
    QWidget *layoutWidget2;
    QVBoxLayout *leftVerticalLayout;
    QLabel *label_2;
    QWidget *graphWidget;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *buttonStartTracking;
    QPushButton *buttonStopTracking;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QDial *dialTrackSpeed;
    QSpinBox *spinTrackSpeed;
    QSpacerItem *horizontalSpacer;
    QLabel *label_4;
    QDial *dialScheduleDevice;
    QSpinBox *spinDeviceSchedule;
    QTableView *tableView;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *buttonInit;
    QPushButton *buttonStart;
    QPushButton *buttonStop;
    QPushButton *buttonDeinit;
    QToolBar *mainToolBar;
    QMenuBar *menuBar;
    QMenu *menuHelp;
    QMenu *menuCommand;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(916, 590);
        MainWindow->setMinimumSize(QSize(832, 560));
        actionShow_Info = new QAction(MainWindow);
        actionShow_Info->setObjectName(QString::fromUtf8("actionShow_Info"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy);
        splitter->setMinimumSize(QSize(0, 0));
        splitter->setSizeIncrement(QSize(0, 0));
        splitter->setFrameShape(QFrame::NoFrame);
        splitter->setMidLineWidth(0);
        splitter->setOrientation(Qt::Horizontal);
        splitter->setOpaqueResize(true);
        splitter->setChildrenCollapsible(true);
        layoutWidget1 = new QWidget(splitter);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        rightVerticalLayout = new QVBoxLayout(layoutWidget1);
        rightVerticalLayout->setSpacing(6);
        rightVerticalLayout->setContentsMargins(11, 11, 11, 11);
        rightVerticalLayout->setObjectName(QString::fromUtf8("rightVerticalLayout"));
        rightVerticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        rightVerticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget1);
        label->setObjectName(QString::fromUtf8("label"));

        rightVerticalLayout->addWidget(label);

        listView = new QListView(layoutWidget1);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setContextMenuPolicy(Qt::CustomContextMenu);

        rightVerticalLayout->addWidget(listView);

        commandButtonHorizontalLayout = new QHBoxLayout();
        commandButtonHorizontalLayout->setSpacing(6);
        commandButtonHorizontalLayout->setObjectName(QString::fromUtf8("commandButtonHorizontalLayout"));
        buttonDeleteDevice = new QPushButton(layoutWidget1);
        buttonDeleteDevice->setObjectName(QString::fromUtf8("buttonDeleteDevice"));

        commandButtonHorizontalLayout->addWidget(buttonDeleteDevice);

        buttonUpdateDeviceList = new QPushButton(layoutWidget1);
        buttonUpdateDeviceList->setObjectName(QString::fromUtf8("buttonUpdateDeviceList"));

        commandButtonHorizontalLayout->addWidget(buttonUpdateDeviceList);


        rightVerticalLayout->addLayout(commandButtonHorizontalLayout);

        splitter->addWidget(layoutWidget1);
        layoutWidget2 = new QWidget(splitter);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        leftVerticalLayout = new QVBoxLayout(layoutWidget2);
        leftVerticalLayout->setSpacing(-1);
        leftVerticalLayout->setContentsMargins(11, 11, 11, 11);
        leftVerticalLayout->setObjectName(QString::fromUtf8("leftVerticalLayout"));
        leftVerticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        leftVerticalLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        leftVerticalLayout->addWidget(label_2);

        graphWidget = new QWidget(layoutWidget2);
        graphWidget->setObjectName(QString::fromUtf8("graphWidget"));

        leftVerticalLayout->addWidget(graphWidget);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        buttonStartTracking = new QPushButton(layoutWidget2);
        buttonStartTracking->setObjectName(QString::fromUtf8("buttonStartTracking"));

        horizontalLayout_6->addWidget(buttonStartTracking);

        buttonStopTracking = new QPushButton(layoutWidget2);
        buttonStopTracking->setObjectName(QString::fromUtf8("buttonStopTracking"));

        horizontalLayout_6->addWidget(buttonStopTracking);


        leftVerticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_3->setContentsMargins(-1, 0, -1, -1);
        label_3 = new QLabel(layoutWidget2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        dialTrackSpeed = new QDial(layoutWidget2);
        dialTrackSpeed->setObjectName(QString::fromUtf8("dialTrackSpeed"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(dialTrackSpeed->sizePolicy().hasHeightForWidth());
        dialTrackSpeed->setSizePolicy(sizePolicy1);
        dialTrackSpeed->setMaximumSize(QSize(40, 40));
        dialTrackSpeed->setMaximum(1000);
        dialTrackSpeed->setValue(1000);

        horizontalLayout_3->addWidget(dialTrackSpeed);

        spinTrackSpeed = new QSpinBox(layoutWidget2);
        spinTrackSpeed->setObjectName(QString::fromUtf8("spinTrackSpeed"));
        spinTrackSpeed->setKeyboardTracking(false);
        spinTrackSpeed->setMinimum(1);
        spinTrackSpeed->setMaximum(1000);
        spinTrackSpeed->setSingleStep(10);
        spinTrackSpeed->setValue(1000);

        horizontalLayout_3->addWidget(spinTrackSpeed);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        label_4 = new QLabel(layoutWidget2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_3->addWidget(label_4);

        dialScheduleDevice = new QDial(layoutWidget2);
        dialScheduleDevice->setObjectName(QString::fromUtf8("dialScheduleDevice"));
        sizePolicy1.setHeightForWidth(dialScheduleDevice->sizePolicy().hasHeightForWidth());
        dialScheduleDevice->setSizePolicy(sizePolicy1);
        dialScheduleDevice->setMaximumSize(QSize(40, 40));
        dialScheduleDevice->setMaximum(1000);
        dialScheduleDevice->setValue(1000);

        horizontalLayout_3->addWidget(dialScheduleDevice);

        spinDeviceSchedule = new QSpinBox(layoutWidget2);
        spinDeviceSchedule->setObjectName(QString::fromUtf8("spinDeviceSchedule"));
        spinDeviceSchedule->setKeyboardTracking(false);
        spinDeviceSchedule->setMinimum(1);
        spinDeviceSchedule->setMaximum(1000);
        spinDeviceSchedule->setSingleStep(10);
        spinDeviceSchedule->setValue(1000);

        horizontalLayout_3->addWidget(spinDeviceSchedule);


        leftVerticalLayout->addLayout(horizontalLayout_3);

        tableView = new QTableView(layoutWidget2);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setMinimumSize(QSize(0, 129));
        tableView->setMaximumSize(QSize(16777215, 150));
        QFont font;
        font.setPointSize(10);
        tableView->setFont(font);
        tableView->setSelectionMode(QAbstractItemView::MultiSelection);

        leftVerticalLayout->addWidget(tableView);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        buttonInit = new QPushButton(layoutWidget2);
        buttonInit->setObjectName(QString::fromUtf8("buttonInit"));

        horizontalLayout_5->addWidget(buttonInit);

        buttonStart = new QPushButton(layoutWidget2);
        buttonStart->setObjectName(QString::fromUtf8("buttonStart"));

        horizontalLayout_5->addWidget(buttonStart);

        buttonStop = new QPushButton(layoutWidget2);
        buttonStop->setObjectName(QString::fromUtf8("buttonStop"));

        horizontalLayout_5->addWidget(buttonStop);

        buttonDeinit = new QPushButton(layoutWidget2);
        buttonDeinit->setObjectName(QString::fromUtf8("buttonDeinit"));

        horizontalLayout_5->addWidget(buttonDeinit);


        leftVerticalLayout->addLayout(horizontalLayout_5);

        leftVerticalLayout->setStretch(1, 1);
        splitter->addWidget(layoutWidget2);

        horizontalLayout_2->addWidget(splitter);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 916, 22));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuCommand = new QMenu(menuBar);
        menuCommand->setObjectName(QString::fromUtf8("menuCommand"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuCommand->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuHelp->addAction(actionShow_Info);

        retranslateUi(MainWindow);
        QObject::connect(dialScheduleDevice, SIGNAL(valueChanged(int)), spinDeviceSchedule, SLOT(setValue(int)));
        QObject::connect(dialTrackSpeed, SIGNAL(valueChanged(int)), spinTrackSpeed, SLOT(setValue(int)));
        QObject::connect(spinDeviceSchedule, SIGNAL(valueChanged(int)), dialScheduleDevice, SLOT(setValue(int)));
        QObject::connect(spinTrackSpeed, SIGNAL(valueChanged(int)), dialTrackSpeed, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionShow_Info->setText(QApplication::translate("MainWindow", "Show Info", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Devices", 0, QApplication::UnicodeUTF8));
        buttonDeleteDevice->setText(QApplication::translate("MainWindow", "Delete Device", 0, QApplication::UnicodeUTF8));
        buttonUpdateDeviceList->setText(QApplication::translate("MainWindow", "Update List", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Selected device", 0, QApplication::UnicodeUTF8));
        buttonStartTracking->setText(QApplication::translate("MainWindow", "Start Tracking", 0, QApplication::UnicodeUTF8));
        buttonStopTracking->setText(QApplication::translate("MainWindow", "Stop Tracking", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Track Seed:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        spinTrackSpeed->setToolTip(QApplication::translate("MainWindow", "milliseconds", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("MainWindow", "Device Schedule:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        spinDeviceSchedule->setToolTip(QApplication::translate("MainWindow", "milliseconds", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        buttonInit->setText(QApplication::translate("MainWindow", "Init", 0, QApplication::UnicodeUTF8));
        buttonStart->setText(QApplication::translate("MainWindow", "Start", 0, QApplication::UnicodeUTF8));
        buttonStop->setText(QApplication::translate("MainWindow", "Stop", 0, QApplication::UnicodeUTF8));
        buttonDeinit->setText(QApplication::translate("MainWindow", "Deinit", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
        menuCommand->setTitle(QApplication::translate("MainWindow", "Command", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
