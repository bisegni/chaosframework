#include "LogDataTableModel.h"

using namespace chaos::metadata_service_client::api_proxy::logging;

LogDataTableModel::LogDataTableModel(QObject *parent):
    ChaosAbstractTableModel(parent) {}

#define FILL_BY_MAP(cntr, iter, map, conv)\
    for(iter it = map.begin();\
    it != map.end();\
    it++) {\
    log_data_map.insert(cntr++, QPair<QString, QVariant>(QString::fromStdString(it->first), conv(it->second)));\
    }\


void LogDataTableModel::setLogEntry(boost::shared_ptr<LogEntry> _log_entry_ptr) {
    beginResetModel();
    int counter = 0;
    FILL_BY_MAP(counter, LoggingKeyValueBoolMapIterator, _log_entry_ptr->map_bool_value, QVariant);
    FILL_BY_MAP(counter, LoggingKeyValueInt32MapIterator, _log_entry_ptr->map_int32_value, QVariant);
    FILL_BY_MAP(counter, LoggingKeyValueInt64MapIterator, _log_entry_ptr->map_int64_value, QVariant);
    FILL_BY_MAP(counter, LoggingKeyValueDoubleMapIterator, _log_entry_ptr->map_double_value, QVariant);
    FILL_BY_MAP(counter, LoggingKeyValueStringMapIterator, _log_entry_ptr->map_string_value, QString::fromStdString);
    endResetModel();
}

void LogDataTableModel::clear() {
    beginResetModel();
    log_data_map.clear();
    endResetModel();
}

int LogDataTableModel::getRowCount() const {

    return log_data_map.size();
}

int LogDataTableModel::getColumnCount() const {
    return 2;
}

QString LogDataTableModel::getHeaderForColumn(int column) const{
    QString result;
    switch(column) {
    case 0:
        result = "Key";
        break;
    case 1:
        result = "Value";
        break;
    }
    return result;
}

QVariant LogDataTableModel::getCellData(int row, int column) const{
    QVariant result;
    switch(column) {
    case 0:
        result = log_data_map[row].first;
        break;
    case 1:
        result = log_data_map[row].second;
        break;
    }
    return result;
}

QVariant LogDataTableModel::getCellUserData(int row, int column) const{
    return getCellData(row,
                       column);
}

QVariant LogDataTableModel::getTooltipTextForData(int row, int column) const{
    return getCellData(row,
                       column);
}

QVariant LogDataTableModel::getTextAlignForData(int row, int column) const{
    return Qt::AlignLeft+Qt::AlignVCenter;
}
