#include "MainController.h"
#include <QApplication>
int main(int argc, char *argv[]) {
    int returnValue = 0;
    QApplication a(argc, argv);
    MainController m;
    if(m.init(argc, (const char **)argv)){
        returnValue = a.exec();
    }
    m.deinit();
    return returnValue;
}
