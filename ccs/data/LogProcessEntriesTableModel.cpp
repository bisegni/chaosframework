#include "LogProcessEntriesTableModel.h"
#include <QDateTime>
#include <limits>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::logging;

LogProcessEntriesTableModel::LogProcessEntriesTableModel(const QString& _node_uid,
                                           QObject *parent):
    ChaosAbstractTableModel(parent),
    ApiHandler(),
    node_uid(_node_uid),
    api_submitter(this),
    number_of_max_result(0){}

//!update log entries for node uid as emitter and log domain list to inclue
void LogProcessEntriesTableModel::updateEntriesList() {
    api_submitter.submitApiResult("LogProcessEntriesTableModel::updateEntriesList",
                                  GET_CHAOS_API_PTR(agent::logging::GetProcessLogEntries)->execute(node_uid.toStdString(),
                                                                                                   number_of_max_result,
                                                                                                   false,
                                                                                                   std::numeric_limits<int64_t>::max()));
}

void LogProcessEntriesTableModel::onApiDone(const QString& tag,
                                     QSharedPointer<CDataWrapper> api_result) {
    //data received
    beginResetModel();


    endResetModel();
    //emit signal that model has changed
    emit(dataChanged(LogProcessEntriesTableModel::index(0,0), LogProcessEntriesTableModel::index(found_entires.size(),2)));

}

void LogProcessEntriesTableModel::clear() {
    beginResetModel();
    endResetModel();
}

void LogProcessEntriesTableModel::setMaxResultItem(uint32_t _number_of_max_result) {
    number_of_max_result = _number_of_max_result;
}

int LogProcessEntriesTableModel::getRowCount() const {
    return found_entires.size();
}

int LogProcessEntriesTableModel::getColumnCount() const {
    return 2;
}

QString LogProcessEntriesTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch (column) {
    case 0:
        result = tr("Timestamp");
        break;
    case 1:
        result = tr("Entry");
        break;
    default:
        break;
    }
    return result;
}

QVariant LogProcessEntriesTableModel::getCellData(int row, int column) const {
    QString result;
    switch (column) {
    case 0:
        result = QDateTime::fromMSecsSinceEpoch(found_entires[row].entry_ts, Qt::LocalTime).toString("dd-MM-yy hh.mm.ss.zzz");
        break;
    case 1:
        result = QString::fromStdString(found_entires[row].entry_log_line);
        break;
    default:
        break;
    }
    return result;
}

QVariant LogProcessEntriesTableModel::getCellUserData(int row, int column) const {
    return getCellData(row, column);
}

QVariant LogProcessEntriesTableModel::getTooltipTextForData(int row, int column) const {
    return getCellData(row, column);
}

QVariant LogProcessEntriesTableModel::getTextAlignForData(int row, int column) const {
    return Qt::AlignLeft+Qt::AlignVCenter;
}

bool LogProcessEntriesTableModel::isCellSelectable(const QModelIndex &index) const {
    return true;
}
