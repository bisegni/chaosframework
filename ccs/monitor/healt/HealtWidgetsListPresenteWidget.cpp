#include "HealtWidgetsListPresenteWidget.h"
#include "ui_HealtWidgetsListPresenteWidget.h"

HealtWidgetsListPresenteWidget::HealtWidgetsListPresenteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HealtWidgetsListPresenteWidget)
{
    ui->setupUi(this);
    ui->scrollAreaWidgetContents->setLayout(new QVBoxLayout(this));
}

HealtWidgetsListPresenteWidget::~HealtWidgetsListPresenteWidget()
{
    delete ui;
}

void HealtWidgetsListPresenteWidget::addHealtWidget(HealtPresenterWidget *h_widget) {
    ((QVBoxLayout*)ui->scrollAreaWidgetContents->layout())->addWidget(h_widget);
    int element_count = ((QVBoxLayout*)ui->scrollAreaWidgetContents->layout())->count();
    ((QVBoxLayout*)ui->scrollAreaWidgetContents->layout())->insertStretch(element_count);
}

void HealtWidgetsListPresenteWidget::removeHealtWidget(HealtPresenterWidget *h_widget) {
    int element_count = ((QVBoxLayout*)ui->scrollAreaWidgetContents->layout())->count();
    QLayoutItem *strecher = ((QVBoxLayout*)ui->scrollAreaWidgetContents->layout())->itemAt(element_count);
    ((QVBoxLayout*)ui->scrollAreaWidgetContents->layout())->removeItem(strecher);
    ((QVBoxLayout*)ui->scrollAreaWidgetContents->layout())->removeWidget(h_widget);
    ((QVBoxLayout*)ui->scrollAreaWidgetContents->layout())->insertStretch(element_count-1);
}
