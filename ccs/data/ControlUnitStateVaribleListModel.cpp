#include "ControlUnitStateVaribleListModel.h"

#include <chaos/common/alarm/alarm.h>

#include <QBrush>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;

ControlUnitStateVaribleListModel::ControlUnitStateVaribleListModel(const QString& _control_unit_id,
                                                                   StateVariableType state_variable_type,
                                                                    QObject *parent):
    control_unit_id(_control_unit_id) {
    setStateVariableType(state_variable_type);

}

ControlUnitStateVaribleListModel::~ControlUnitStateVaribleListModel() {}

void ControlUnitStateVaribleListModel::track() {
    ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(control_unit_id.toStdString(),
                                                                       chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit,
                                                                       this);
}

void ControlUnitStateVaribleListModel::untrack() {
    ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(control_unit_id.toStdString(),
                                                                          chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit,
                                                                          this);
}

void ControlUnitStateVaribleListModel::setStateVariableType(StateVariableType type) {
    switch(type){
    case StateVariableTypeAlarmCU:
        chaos_dataset_type = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_WARNING;
        break;
    case StateVariableTypeAlarmDEV:
        chaos_dataset_type = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_ALARM;
        break;
    default:
        chaos_dataset_type = -1;
    }
}

QVariant ControlUnitStateVaribleListModel::getBackgroudColor(int row) const {
    const std::string alarm_name = alarm_names[row].toStdString();
    MapDatasetKeyValuesConstIterator it = alarm_dataset.find(alarm_name);
    if(it == alarm_dataset.end()) return QVariant();

    switch(it->second.asInt32()) {
    case chaos::common::alarm::MultiSeverityAlarmLevelClear:
        return QVariant();
        break;
    case chaos::common::alarm::MultiSeverityAlarmLevelWarning:
        return QBrush(Qt::yellow);
        break;
    case chaos::common::alarm::MultiSeverityAlarmLevelHigh:
        return QBrush(Qt::red);
        break;
    }
    return QVariant();
}

QVariant ControlUnitStateVaribleListModel::getTextColor(int row) const {
    const std::string alarm_name = alarm_names[row].toStdString();
    MapDatasetKeyValuesConstIterator it = alarm_dataset.find(alarm_name);
    if(it == alarm_dataset.end()) return QVariant();

    switch(it->second.asInt32()) {
    case chaos::common::alarm::MultiSeverityAlarmLevelClear:
        return QVariant();
        break;
    case chaos::common::alarm::MultiSeverityAlarmLevelWarning:
        return QBrush(Qt::black);
        break;
    case chaos::common::alarm::MultiSeverityAlarmLevelHigh:
        return QBrush(Qt::black);
        break;
    }
    return QVariant();
}

int ControlUnitStateVaribleListModel::getRowCount() const {
    return alarm_names.size();
}

QVariant ControlUnitStateVaribleListModel::getRowData(int row) const {
    return alarm_names[row];
}

QVariant ControlUnitStateVaribleListModel::getUserData(int row) const {
    return alarm_names[row];
}

bool ControlUnitStateVaribleListModel::isRowCheckable(int row) const {
    return false;
}

Qt::CheckState ControlUnitStateVaribleListModel::getCheckableState(int row)const {
    return Qt::Unchecked;
}

bool ControlUnitStateVaribleListModel::setRowCheckState(const int row, const QVariant& value) {
    return false;
}

void ControlUnitStateVaribleListModel::updatedDS(const std::string& control_unit_uid,
                               int dataset_type,
                               MapDatasetKeyValues& dataset_key_values) {
    if(dataset_type != chaos_dataset_type) return;
    beginResetModel();
    alarm_names.clear();
    alarm_dataset = dataset_key_values;
    for(MapDatasetKeyValuesIterator it = dataset_key_values.begin(),
        end = dataset_key_values.end();
        it != end;
        it++) {
        if(it->first.compare(chaos::DataPackCommonKey::DPCK_DEVICE_ID) == 0 ||
                it->first.compare(chaos::DataPackCommonKey::DPCK_TIMESTAMP) == 0 ||
                it->first.compare(chaos::DataPackCommonKey::DPCK_SEQ_ID) == 0 ||
                it->first.compare(chaos::DataPackCommonKey::DPCK_DATASET_TYPE) == 0) {
            continue;
        }
        alarm_names.push_back(QString::fromStdString(it->first));
    }
    endResetModel();
}

void ControlUnitStateVaribleListModel::noDSDataFound(const std::string& control_unit_uid,
                                   int dataset_type) {
    if(dataset_type != chaos::DataPackCommonKey::DPCK_DATASET_TYPE_ALARM) return;
    beginResetModel();
    alarm_names.clear();
    alarm_dataset.clear();
    endResetModel();
}
