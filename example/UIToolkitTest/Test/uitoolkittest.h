#ifndef UITOOLKITTEST_H
#define UITOOLKITTEST_H

#include <QMainWindow>

namespace Ui {
class UIToolkitTest;
}

class UIToolkitTest : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit UIToolkitTest(QWidget *parent = 0);
    ~UIToolkitTest();
    
private:
    Ui::UIToolkitTest *ui;
};

#endif // UITOOLKITTEST_H
