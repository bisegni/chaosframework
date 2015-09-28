
#include "MainController.h"
#include "MainWindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainController m;
    m.init(argc, argv, a);
    int returnValue = a.exec();
    m.deinit();
    return returnValue;
}
