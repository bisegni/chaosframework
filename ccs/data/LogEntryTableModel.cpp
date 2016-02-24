#include "LogEntryTableModel.h"

#include <QDateTime>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::logging;

LogEntryTableModel::LogEntryTableModel(QObject *parent):
    ChaosAbstractTableModel(parent),
    ApiHandler(),
    api_submitter(this),
    number_of_max_result(0){}

//!update log entries for node uid as emitter and log domain list to inclue
void LogEntryTableModel::updateEntriesList(const QString& _node_uid,
                                           const LogDomainList& _domain_list) {
    node_uid = _node_uid;
    domain_list = _domain_list;
    api_submitter.submitApiResult("LogEntryTableModel::load_entry_list",
                                  GET_CHAOS_API_PTR(logging::GetLogForSourceUID)->execute(node_uid.toStdString(),
                                                                                          domain_list,
                                                                                          0,
                                                                                          number_of_max_result));
}

void LogEntryTableModel::onApiDone(const QString& tag,
                                   QSharedPointer<CDataWrapper> api_result) {
    //data received
    beginResetModel();
    helper = logging::GetLogForSourceUID::getHelper(api_result.data());
    //manage sequence id
    endResetModel();
    //emit signal that model has changed
    emit(dataChanged(LogEntryTableModel::index(0,0), LogEntryTableModel::index(helper->getLogEntryListSize(),3)));

}

void LogEntryTableModel::clear() {
    beginResetModel();
    helper.reset();
    endResetModel();
}

boost::shared_ptr<logging::LogEntry> LogEntryTableModel::getLogEntryForRow(unsigned int row) {
    if(row >= helper->getLogEntryListSize()) return boost::shared_ptr<logging::LogEntry>();
    return helper->getLogEntryList()[row];
}

void LogEntryTableModel::setMaxResultItem(uint32_t _number_of_max_result) {
    number_of_max_result = _number_of_max_result;
}

int LogEntryTableModel::getRowCount() const {
    return helper.get()?helper->getLogEntryListSize():0;
}

int LogEntryTableModel::getColumnCount() const {
    return 4;
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
    case 3:
        result = tr("Subject");
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
    case 3:
        result = QString::fromStdString(helper->getLogEntryList()[row]->subject);
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
    case 3:
        result = tr("Subject");
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
