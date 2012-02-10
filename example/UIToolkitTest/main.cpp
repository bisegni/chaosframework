#include <QtGui/QApplication>
#include "mainwindow.h"
#include <chaos/ui_toolkit/ChaosUIToolkit.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    chaos::ui::ChaosUIToolkit::getInstance()->init(argc, argv);
    MainWindow w;
    w.show();
    int returnValue = a.exec();
    chaos::ui::ChaosUIToolkit::getInstance()->deinit();
    return returnValue;
}
