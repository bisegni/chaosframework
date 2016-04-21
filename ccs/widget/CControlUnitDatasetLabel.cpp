#include "CControlUnitDatasetLabel.h"

CControlUnitDatasetLabel::CControlUnitDatasetLabel(QWidget *parent) :
    QLabel(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this) {

}

CControlUnitDatasetLabel::~CControlUnitDatasetLabel() {

}

void CControlUnitDatasetLabel::initChaosContent() {
    trackNode();
}

void CControlUnitDatasetLabel::deinitChaosContent() {
    untrackNode();
}

void CControlUnitDatasetLabel::updateChaosContent() {

}

void CControlUnitDatasetLabel::setDatasetType(CControlUnitDatasetLabel::DatasetType dataset_type) {
    p_dataset_type = dataset_type;
}

CControlUnitDatasetLabel::DatasetType CControlUnitDatasetLabel::datasetType() {
    return p_dataset_type;
}

void CControlUnitDatasetLabel::setDatasetAttributeName(const QString& datasetAttributeName) {
    p_dataset_attribute_name = datasetAttributeName;
}

QString CControlUnitDatasetLabel::datasetAttributeName() {
    return p_dataset_attribute_name;
}

void CControlUnitDatasetLabel::setDoublePrintPrecision(int doublePrintPrecision) {
    p_double_print_precision = doublePrintPrecision;
}

int CControlUnitDatasetLabel::doublePrintPrecision() {
    return p_double_print_precision;
}

void CControlUnitDatasetLabel::nodeChangedOnlineState(const std::string& node_uid,
                                                      chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                                      chaos::metadata_service_client::node_monitor::OnlineState new_state) {
    online_state = new_state;
    QMetaObject::invokeMethod(this,
                              "updateUI",
                              Qt::QueuedConnection,
                              Q_ARG(QString, text()));
}

void CControlUnitDatasetLabel::updatedDS(const std::string& control_unit_uid,
                                         int dataset_type,
                                         chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values) {
    if(dataset_type != datasetType()) return;

    qDebug()<< "Dataset received!";

    //we can update the daset output variable
    QString text_to_show = datasetValueToLabel(datasetAttributeName(),
                                               dataset_key_values,
                                               doublePrintPrecision());

    QMetaObject::invokeMethod(this,
                              "updateUI",
                              Qt::QueuedConnection,
                              Q_ARG(QString, text_to_show));
}

void CControlUnitDatasetLabel::noDSDataFound(const std::string& control_unit_uid,
                                             int dataset_type) {

}

void CControlUnitDatasetLabel::updateUI(const QString& label_text) {
    setText(label_text);
    setEnabled(online_state == chaos::metadata_service_client::node_monitor::OnlineStateON);
    setStyleSheetColorForOnlineState(online_state, this);
}
