#include "LogBrowser.h"
#include "ui_LogBrowser.h"

#include <QIntValidator>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy::logging;

LogBrowser::LogBrowser() :
    PresenterWidget(NULL),
    ui(new Ui::LogBrowser) {
    ui->setupUi(this);
    QList<int> sizes;
    sizes << (size().width()*1/5) << (size().width()*4/5);
    ui->splitterResults->setSizes(sizes);
}

LogBrowser::~LogBrowser() {
    delete ui;
}

void LogBrowser::initUI() {
    setTabTitle("Log Browser");
    //configure domain list
    ui->listViewAllLogDomain->setModel(&log_domain_list_model);
    connect(&log_domain_list_model,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            SLOT(logTypesDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    //configure result tables entries
    log_entry_table_model.setMaxResultItem(ui->lineEditMaxResult->text().toInt());
    ui->tableViewFoundEntry->setModel(&log_entry_table_model);
    ui->tableViewFoundEntry->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewFoundEntry->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewFoundEntry->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewFoundEntry->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->tableViewFoundEntry->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(logEntriesTableSelectionChanged(QModelIndex,QModelIndex)));

    ui->tableViewEntryData->setModel(&log_data_table_model);
    ui->tableViewEntryData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewEntryData->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewEntryData->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewEntryData->setSelectionMode(QAbstractItemView::SingleSelection);

    //configure max result line edit
    ui->lineEditMaxResult->setValidator(new QIntValidator(1, 1000, this));

    //configure the autorefresh
    ui->lineEditAutorefreshDelay->setValidator(new QIntValidator(1, 60, this));
    autoupdate_timer.setInterval(ui->lineEditAutorefreshDelay->text().toInt() * 1000);
    connect(&autoupdate_timer,
            SIGNAL(timeout()),
            SLOT(on_pushButtonStartSearch_clicked()));
    //update all view
    updateAll();
}

bool LogBrowser::isClosing() {
    autoupdate_timer.stop();
    return true;
}

void LogBrowser::updateAll() {
    //update all domain in all log entry
    log_domain_list_model.updateDomainListForUID();
}

void LogBrowser::logTypesDataChanged(const QModelIndex& top_left,
                                     const QModelIndex& bottom_right,
                                     const QVector<int>& roles) {
    //some domain has been checked or unchecked
    on_pushButtonStartSearch_clicked();
}

void LogBrowser::on_checkBoxSearchDate_clicked() {
    if(ui->checkBoxSearchDate->isChecked()) {
        QDateTime current_time =  QDateTime::currentDateTime();
        ui->dateTimeEditStartDate->setDateTime(current_time);
        ui->dateTimeEditEndDate->setDateTime(current_time);
    } else {
        ui->dateTimeEditStartDate->clear();
        ui->dateTimeEditEndDate->clear();
    }
}

void LogBrowser::on_pushButtonStartSearch_clicked() {
    LogDomainList domain_list;
    uint64_t start_ts = 0;
    uint64_t end_ts = 0;

    if(ui->checkBoxSearchDate->isChecked()) {
        start_ts = ui->dateTimeEditStartDate->dateTime().toUTC().toMSecsSinceEpoch();
        end_ts = ui->dateTimeEditEndDate->dateTime().toUTC().toMSecsSinceEpoch();
    }
    log_domain_list_model.getActiveDomains(domain_list);
    log_entry_table_model.updateEntriesList(ui->lineEditSearchText->text(),
                                            domain_list,
                                            start_ts,
                                            end_ts);
}

void LogBrowser::logEntriesTableSelectionChanged(const QModelIndex& current_selection,
                                                 const QModelIndex& previous_selection) {
    if(current_selection.isValid()) {
        log_data_table_model.setLogEntry(log_entry_table_model.getLogEntryForRow(current_selection.row()));
    } else {
        log_data_table_model.clear();
    }
}

void LogBrowser::on_lineEditAutorefreshDelay_editingFinished() {
    autoupdate_timer.setInterval(ui->lineEditAutorefreshDelay->text().toInt() * 1000);
}

void LogBrowser::on_pushButton_clicked() {
    updateAll();
}

void LogBrowser::on_lineEditMaxResult_editingFinished() {
    log_entry_table_model.setMaxResultItem(ui->lineEditMaxResult->text().toUInt());
}

void LogBrowser::on_checkBoxAutoUpdate_clicked() {
    bool enable = ui->checkBoxAutoUpdate->isChecked();
    if(enable){
        autoupdate_timer.start();
    } else {
        autoupdate_timer.stop();
    }
}
