#include "CNodeLogWidget.h"
#include "ui_CNodeLogWidget.h"

CNodeLogWidget::CNodeLogWidget(QWidget *parent):
    QWidget(parent),
    ChaosWidgetCompanion(),
    ui(new Ui::CNodeLogWidget) {
    ui->setupUi(this);
    QList<int> sizes;
    sizes << 100 << 150 << 200;
    ui->splitterMainHorizontal->setSizes(sizes);
}

CNodeLogWidget::~CNodeLogWidget() {
    delete ui;
}

void CNodeLogWidget::initChaosContent() {
    //add the list model to log domain list
    ui->listViewLogTypes->setModel(&domain_list_model);
}

void CNodeLogWidget::deinitChaosContent() {

}

void CNodeLogWidget::updateChaosContent() {
    domain_list_model.updateDomainListForUID(getNodeUID());
}
