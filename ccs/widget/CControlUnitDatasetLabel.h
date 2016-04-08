#ifndef CCONTROLUNITDATASETLABEL_H
#define CCONTROLUNITDATASETLABEL_H

#include "ChaosMonitorWidgetCompanion.h"

#include <QLabel>
#include <QString>

class CControlUnitDatasetLabel :
        public QLabel,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    Q_OBJECT
public:
    enum DatasetType {
        DatasetTypeOutput = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT,
        DatasetTypeInput = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT,
        DatasetTypeSystem = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM
    };

    explicit CControlUnitDatasetLabel(QWidget *parent = 0);
    ~CControlUnitDatasetLabel();

    void setDatasetType(DatasetType dataset_type);
    DatasetType datasetType();
protected:
    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                chaos::metadata_service_client::node_monitor::OnlineState new_state);
    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);

    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);

private:
    Q_ENUM(DatasetType)
    Q_PROPERTY(DatasetType dataset_type READ datasetType WRITE setDatasetType)
    DatasetType p_dataset_type;
};

#endif // CCONTROLUNITDATASETLABEL_H
