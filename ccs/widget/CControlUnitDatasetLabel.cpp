#include "CControlUnitDatasetLabel.h"

CControlUnitDatasetLabel::CControlUnitDatasetLabel(QWidget *parent) :
    QLabel(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this) {

}

CControlUnitDatasetLabel::~CControlUnitDatasetLabel() {

}

void CControlUnitDatasetLabel::setDatasetType(CControlUnitDatasetLabel::DatasetType dataset_type) {
    p_dataset_type = dataset_type;
}

CControlUnitDatasetLabel::DatasetType CControlUnitDatasetLabel::datasetType() {
    return p_dataset_type;
}

void CControlUnitDatasetLabel::nodeChangedOnlineState(const std::string& node_uid,
                            chaos::metadata_service_client::node_monitor::OnlineState old_state,
                            chaos::metadata_service_client::node_monitor::OnlineState new_state) {

}

void CControlUnitDatasetLabel::updatedDS(const std::string& control_unit_uid,
               int dataset_type,
               chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values) {
    if(dataset_type != datasetType()) return;

    //we can update the daset output variable
}

void CControlUnitDatasetLabel::noDSDataFound(const std::string& control_unit_uid,
                   int dataset_type) {

}
