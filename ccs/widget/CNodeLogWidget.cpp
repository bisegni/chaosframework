#include "CNodeLogWidget.h"
#include "ui_CNodeLogWidget.h"

#include <QDebug>
#include <QMessageBox>
#include <QIntValidator>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

CNodeLogWidget::CNodeLogWidget(QWidget *parent):
    QWidget(parent),
    ChaosWidgetCompanion(),
    ui(new Ui::CNodeLogWidget) {
    ui->setupUi(this);
    QList<int> sizes;
    sizes << (size().width()*1/5) << (size().width()*4/5);
    ui->splitterMainHorizontal->setSizes(sizes);
}

CNodeLogWidget::~CNodeLogWidget() {
    delete ui;
}

void CNodeLogWidget::initChaosContent() {
    //add the list model to log domain list
    ui->lineEditMaxNumberOfResult->setValidator(new QIntValidator(1, 1000, this));
    setMaxResultItem(ui->lineEditMaxNumberOfResult->text().toUInt());

    ui->listViewLogTypes->setModel(&domain_list_model);
    //connect for signal when user check or uncheck the log domains
    connect(&domain_list_model,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            SLOT(logTypesDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    //configure the entry table
    ui->tableViewLogEntries->setModel(&entry_table_model);
    ui->tableViewLogEntries->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewLogEntries->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewLogEntries->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewLogEntries->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->tableViewLogEntries->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(logEntriesTableSelectionChanged(QModelIndex,QModelIndex)));

    //set table view for log data
    ui->tableViewLogData->setModel(&data_table_model);
    ui->tableViewLogData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewLogData->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewLogData->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewLogData->setSelectionMode(QAbstractItemView::MultiSelection);

    //setup refresh delay
    ui->lineEditRefreshUpdate->setValidator(new QIntValidator(10, 60, this));
    refresh_timer.setTimerType(Qt::VeryCoarseTimer);
    refresh_timer.setInterval(ui->lineEditRefreshUpdate->text().toInt()*1000);
    connect(&refresh_timer,
            SIGNAL(timeout()),
            SLOT(timeoutUpdateTimer()));
    //register for log
    ChaosMetadataServiceClient::getInstance()->registerEventHandler(this);
}

void CNodeLogWidget::deinitChaosContent() {
    //remove log handler
    ChaosMetadataServiceClient::getInstance()->deregisterEventHandler(this);

    //stop timer
    refresh_timer.start();
}

void CNodeLogWidget::updateChaosContent() {
    domain_list_model.updateDomainListForUID(nodeUID());
}

void CNodeLogWidget::setMaxResultItem(qint32 max_result_item) {
    entry_table_model.setMaxResultItem(p_max_result_item = max_result_item);
}

qint32 CNodeLogWidget::maxResultItem() {
    return p_max_result_item;
}

void CNodeLogWidget::handleLogEvent(const std::string source,
                                    const std::string domain) {
    //if widget i snot visible we do nothing
    if(isVisible() == false) return;

    if(source.compare(nodeUID().toStdString()) == 0) {
        qDebug()<< "Received event for this node" << nodeUID();
        //the event if for my node
        if(domain_list_model.isDomainChecked(QString::fromStdString(domain))) {
            qDebug()<< "Update event for log";
            //we need to update the log
            domain_list_model.updateDomainListForUID(nodeUID());
        } else if(domain_list_model.isDomainPresent(QString::fromStdString(domain)) == false) {
            qDebug()<< "Update event for log - update domain list";
            //we need to reload all to get new domain
            domain_list_model.updateDomainListForUID(nodeUID());
        }
    }
}

void CNodeLogWidget::updateEntryList() {
    if(!isVisible()) return;
    logging::LogDomainList domain_list;
    domain_list_model.getActiveDomains(domain_list);
    entry_table_model.updateEntriesList(nodeUID(),
                                        domain_list);
}

void CNodeLogWidget::logTypesDataChanged(const QModelIndex& top_left,
                                         const QModelIndex& bottom_right,
                                         const QVector<int>& roles) {
    //some domain has been checked or unchecked
    updateEntryList();
}

void CNodeLogWidget::logEntriesTableSelectionChanged(const QModelIndex& current_selection,
                                                     const QModelIndex& previous_selection) {
    if(current_selection.isValid()) {
        data_table_model.setLogEntry(entry_table_model.getLogEntryForRow(current_selection.row()));
    } else {
        data_table_model.clear();
    }
}

void CNodeLogWidget::on_pushButtonUpdateLogTypes_clicked() {
    domain_list_model.updateDomainListForUID(nodeUID());
}

void CNodeLogWidget::on_lineEditMaxNumberOfResult_returnPressed() {
    setMaxResultItem(ui->lineEditMaxNumberOfResult->text().toUInt());
    updateEntryList();
}

void CNodeLogWidget::on_lineEditRefreshUpdate_editingFinished() {
    //update delay
    refresh_timer.setInterval(ui->lineEditRefreshUpdate->text().toInt()*1000);
}

void CNodeLogWidget::timeoutUpdateTimer() {
    updateEntryList();
    qDebug() << "Timer timout";
}

void CNodeLogWidget::on_checkBoxAutoUpdate_clicked() {
    bool enable = ui->checkBoxAutoUpdate->isChecked();
    if(enable){refresh_timer.start();} else {refresh_timer.stop();}
}
