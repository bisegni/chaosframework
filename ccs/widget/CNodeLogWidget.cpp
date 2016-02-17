#include "CNodeLogWidget.h"
#include "ui_CNodeLogWidget.h"

CNodeLogWidget::CNodeLogWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CNodeLogWidget)
{
    ui->setupUi(this);
}

CNodeLogWidget::~CNodeLogWidget()
{
    delete ui;
}
