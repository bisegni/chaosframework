#include "uitoolkittest.h"
#include "ui_uitoolkittest.h"

UIToolkitTest::UIToolkitTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UIToolkitTest)
{
    ui->setupUi(this);
}

UIToolkitTest::~UIToolkitTest()
{
    delete ui;
}
