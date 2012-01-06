#include <QtGui/QApplication>
#include "uitoolkittest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UIToolkitTest w;
    w.show();
    
    return a.exec();
}
