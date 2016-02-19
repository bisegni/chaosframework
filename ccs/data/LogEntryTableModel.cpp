#include "LogEntryTableModel.h"

#include <QDateTime>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::logging;

LogEntryTableModel::LogEntryTableModel(QObject *parent):
    ChaosAbstractTableModel(parent),
    ApiHandler(),
    api_submitter(this),
    page_lenght(30),
    last_received_sequence_id(0){

}

//!update log entries for node uid as emitter and log domain list to inclue
void LogEntryTableModel::updateEntries(const QString& node_uid,
                                       const LogDomainList& domain_list) {
    api_submitter.submitApiResult("LogEntryTableModel::load_entry_list",
                                  GET_CHAOS_API_PTR(logging::GetLogForSourceUID)->execute(node_uid.toStdString(),
                                                                                          domain_list,
                                                                                          last_received_sequence_id,
                                                                                          page_lenght));
}

void LogEntryTableModel::onApiDone(const QString& tag,
                                   QSharedPointer<CDataWrapper> api_result) {
    //data received
    beginResetModel();
    helper = logging::GetLogForSourceUID::getHelper(api_result.data());
    endResetModel();
    //emit signal that model has changed
    emit(dataChanged(LogEntryTableModel::index(0,0), LogEntryTableModel::index(helper->getLogEntryListSize(),3)));
}

void LogEntryTableModel::clear() {
    beginResetModel();
    helper.reset();
    last_received_sequence_id = 0;
    endResetModel();
}

boost::shared_ptr<logging::LogEntry> LogEntryTableModel::getLogEntryForRow(unsigned int row) {
    if(row >= helper->getLogEntryListSize()) return boost::shared_ptr<logging::LogEntry>();
    return helper->getLogEntryList()[row];
}

void LogEntryTableModel::nextPage() {

}

void LogEntryTableModel::previousPage() {

}

int LogEntryTableModel::getRowCount() const {
    return helper.get()?helper->getLogEntryListSize():0;
}

int LogEntryTableModel::getColumnCount() const {
    return 3;
}

QString LogEntryTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch (column) {
    case 0:
        result = tr("Timestamp");
        break;
    case 1:
        result = tr("Emitter");
        break;
    case 2:
        result = tr("Domain");
        break;
    default:
        break;
    }
    return result;
}

QVariant LogEntryTableModel::getCellData(int row, int column) const {
    QString result;
    switch (column) {
    case 0:
        result = QDateTime::fromMSecsSinceEpoch(helper->getLogEntryList()[row]->ts, Qt::LocalTime).toString();
        break;
    case 1:
        result = QString::fromStdString(helper->getLogEntryList()[row]->source_identifier);
        break;
    case 2:
        result = QString::fromStdString(helper->getLogEntryList()[row]->domain);
        break;
    default:
        break;
    }
    return result;
}

QVariant LogEntryTableModel::getCellUserData(int row, int column) const {
    QString result;
    switch (column) {
    case 0:
        result = tr("Timestamp");
        break;
    case 1:
        result = tr("Emitter");
        break;
    case 2:
        result = tr("Domain");
        break;
    default:
        break;
    }
    return result;
}

QVariant LogEntryTableModel::getTooltipTextForData(int row, int column) const {
    QString result;
    switch (column) {
    case 0:
        result = tr("The timestamp when was emitted the entry");
        break;
    case 1:
        result = tr("The source of the entry");
        break;
    case 2:
        result = tr("The log domain of the entry");
        break;
    default:
        break;
    }
    return result;
}

QVariant LogEntryTableModel::getTextAlignForData(int row, int column) const {
    return Qt::AlignLeft+Qt::AlignVCenter;
}

bool LogEntryTableModel::isCellSelectable(const QModelIndex &index) const {
    return true;
}
