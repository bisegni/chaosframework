#ifndef ALARMLISTMODEL_H
#define ALARMLISTMODEL_H

#include "ChaosAbstractListModel.h"

#include <QVector>

class ControlUnitStateVaribleListModel:
        public ChaosAbstractListModel,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
protected:
    int getRowCount() const;
    QVariant getBackgroudColor(int row) const;
    QVariant getTextColor(int row) const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
    bool isRowCheckable(int row) const;
    Qt::CheckState getCheckableState(int row)const;
    bool setRowCheckState(const int row, const QVariant& value);
    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);
    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    Qt::DropActions supportedDragActions() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
public:
    typedef enum StateVariableType {
        StateVariableTypeUndefined,
        StateVariableTypeAlarmCU,
        StateVariableTypeAlarmDEV
    } StateVariableType;

    ControlUnitStateVaribleListModel(const QString& control_unit_id,
                                     StateVariableType state_variable_type,
                                     QObject *parent=0);
    ~ControlUnitStateVaribleListModel();

    void track();
    void untrack();
    void setStateVariableType(StateVariableType type);
private:
    StateVariableType type;
    int chaos_dataset_type;
    const QString control_unit_id;
    QVector<QString> alarm_names;
    chaos::metadata_service_client::node_monitor::MapDatasetKeyValues alarm_dataset;
};

#endif // ALARMLISTMODEL_H
