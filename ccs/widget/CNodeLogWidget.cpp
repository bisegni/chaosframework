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
    //connect for signal when user check or uncheck the log domains
    connect(&domain_list_model,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            SLOT(logTypesDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    //configure the entry table
    ui->tableViewLogEntries->setModel(&entry_table_model);
}

void CNodeLogWidget::deinitChaosContent() {

}

void CNodeLogWidget::updateChaosContent() {
    domain_list_model.updateDomainListForUID(getNodeUID());

}

void CNodeLogWidget::logTypesDataChanged(const QModelIndex& top_left,
                                         const QModelIndex& bottom_right,
                                         const QVector<int>& roles) {
    //some domain has been checked or unchecked
    entry_table_model.updateEntries(getNodeUID(),
                                    domain_list_model.getActiveDomains());

}
