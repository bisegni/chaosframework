#include "ChaosStorageTypeWidget.h"
#include "ui_ChaosStorageTypeWidget.h"
#include <QDebug>

using namespace chaos::DataServiceNodeDefinitionType;
using namespace chaos::metadata_service_client::node_monitor;

ChaosStorageTypeWidget::ChaosStorageTypeWidget(QWidget *parent) :
    QWidget(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this),
    ui(new Ui::ChaosStorageTypeWidget),
    data_found(false){
    ui->setupUi(this);
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

ChaosStorageTypeWidget::~ChaosStorageTypeWidget() {
    deinitChaosContent();
    delete ui;
}

void ChaosStorageTypeWidget::initChaosContent() {
    trackNode();
}

void ChaosStorageTypeWidget::deinitChaosContent() {
    untrackNode();
}

void ChaosStorageTypeWidget::updateChaosContent() {

}

void ChaosStorageTypeWidget::nodeChangedOnlineState(const std::string& node_uid,
                                                    OnlineState old_status,
                                                    OnlineState new_status) {
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
    online_status = new_status;
}

void ChaosStorageTypeWidget::updatedDS(const std::string& control_unit_uid,
                                       int dataset_type,
                                       MapDatasetKeyValues& dataset_key_values) {
    data_found = true;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
    if(dataset_type == DatasetTypeSystem) {
        if(dataset_key_values.count(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE)) {
            qDebug() << "DatasetTypeSystem " << chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE << " with value " << dataset_key_values[chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE].asInt32();
            storage_type = static_cast<chaos::DataServiceNodeDefinitionType::DSStorageType>(dataset_key_values[chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE].asInt32());
        } else {
            storage_type = DSStorageTypeUndefined;
            qDebug() << "DatasetTypeSystem " << chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE << " not found ";

        }
    }
}

void ChaosStorageTypeWidget::noDSDataFound(const std::string& control_unit_uid,
                                           int dataset_type) {
    data_found = false;
    storage_type = DSStorageTypeUndefined;
}

void ChaosStorageTypeWidget::updateUIStatus() {
    //set enable according to online status
    setEnabled(online_status == OnlineStateON);

    //take on off ccording to storage type
    ui->pushButtonLive->setButtonState(storage_type == DSStorageTypeLive);
    ui->pushButtonHistory->setButtonState(storage_type == DSStorageTypeHistory);
    ui->pushButtonLiveAndHistory->setButtonState(storage_type == DSStorageTypeLiveHistory);
    ui->pushButtonUndefined->setButtonState(storage_type == DSStorageTypeUndefined);
}

void ChaosStorageTypeWidget::on_pushButtonLive_clicked() {
    sendStorageType(DSStorageTypeLive);
}

void ChaosStorageTypeWidget::on_pushButtonHistory_clicked() {
    sendStorageType(DSStorageTypeHistory);
}

void ChaosStorageTypeWidget::on_pushButtonLiveAndHistory_clicked() {
    sendStorageType(DSStorageTypeLiveHistory);
}

void ChaosStorageTypeWidget::on_pushButtonUndefined_clicked() {
    sendStorageType(DSStorageTypeUndefined);
}

void ChaosStorageTypeWidget::sendStorageType(chaos::DataServiceNodeDefinitionType::DSStorageType type) {
    chaos::metadata_service_client::api_proxy::node::NodePropertyGroupList property_list;
    boost::shared_ptr<chaos::common::data::CDataWrapperKeyValueSetter> storage_type(new chaos::common::data::CDataWrapperInt32KeyValueSetter(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE,
                                                                                                                                             type));
    boost::shared_ptr<chaos::metadata_service_client::api_proxy::node::NodePropertyGroup> cu_property_group(new chaos::metadata_service_client::api_proxy::node::NodePropertyGroup());
    cu_property_group->group_name = "property_abstract_control_unit";
    cu_property_group->group_property_list.push_back(storage_type);

    property_list.push_back(cu_property_group);

    submitApiResult("CSTW_SET_STORAGE_TYPE",
                    GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::UpdateProperty)->execute(nodeUID().toStdString(),
                                                                                                                property_list));
}

void ChaosStorageTypeWidget::apiHasStarted(const QString& api_tag) {
    setEnabled(false);
}

void ChaosStorageTypeWidget::apiHasEnded(const QString& api_tag) {
    setEnabled(true);
}
