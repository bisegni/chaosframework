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
        DatasetTypeSystem = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM,
        DatasetTypeHealth = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH
    };

    explicit CControlUnitDatasetLabel(QWidget *parent = 0);
    ~CControlUnitDatasetLabel();

    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();

    void setDatasetType(DatasetType dataset_type);
    DatasetType datasetType();

    void setDatasetAttributeName(const QString& datasetAttributeName);
    QString datasetAttributeName();

    void setDoublePrintPrecision(int doublePrintPrecision);
    int doublePrintPrecision();
protected:
    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                chaos::metadata_service_client::node_monitor::OnlineState new_state);
    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);

    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);

protected slots:
    void updateUI(const QString& label_text);

private:
    chaos::metadata_service_client::node_monitor::OnlineState online_state;

    Q_ENUM(DatasetType)
    Q_PROPERTY(DatasetType dataset_type READ datasetType WRITE setDatasetType)
    DatasetType p_dataset_type;

    Q_PROPERTY(QString dataset_attribute_name READ datasetAttributeName WRITE setDatasetAttributeName)
    QString p_dataset_attribute_name;

    Q_PROPERTY(int doublePrintPrecision READ doublePrintPrecision WRITE setDoublePrintPrecision)
    int p_double_print_precision;
};

#endif // CCONTROLUNITDATASETLABEL_H
