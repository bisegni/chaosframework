#ifndef ALARMLISTMODEL_H
#define ALARMLISTMODEL_H

#include "ChaosAbstractListModel.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QVector>

class AlarmListModel:
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
public:
    AlarmListModel(const QString& control_unit_id,
                   QObject *parent=0);
    ~AlarmListModel();

    void track();
    void untrack();
private:
    const QString control_unit_id;
    QVector<QString> alarm_names;
    chaos::metadata_service_client::node_monitor::MapDatasetKeyValues alarm_dataset;
};

#endif // ALARMLISTMODEL_H
