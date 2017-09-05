#include "CPropertyPushButton.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::node_monitor;

CPropertyPushButton::CPropertyPushButton(QWidget *parent):
CStateVisiblePushButton(parent),
ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this),
last_error_code(0),
online_status(OnlineStateUnknown){
    addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    setCheckable(true);
    connect(this,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

CPropertyPushButton::~CPropertyPushButton() {
    deinitChaosContent();
}

void CPropertyPushButton::initChaosContent() {
    trackNode();
}

void CPropertyPushButton::deinitChaosContent() {
    untrackNode();
}

void CPropertyPushButton::updateChaosContent() {

}

void CPropertyPushButton::setPropertyGroupName(QString _property_group_name) {
    property_group_name = _property_group_name;
}

void CPropertyPushButton::setPropertyName(QString _property_name) {
    property_name = _property_name;
}

void CPropertyPushButton::nodeChangedOnlineState(const std::string& node_uid,
                                                 OnlineState old_status,
                                                 OnlineState new_status) {
    online_status = new_status;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

void CPropertyPushButton::updatedDS(const std::string& control_unit_uid,
                                    int dataset_type,
                                    MapDatasetKeyValues& dataset_key_values) {
    if(dataset_type == DatasetTypeSystem) {
        if(dataset_key_values.count(property_name.toStdString())) {
            last_got_value = dataset_key_values[property_name.toStdString()];
            QMetaObject::invokeMethod(this,
                                      "updateUIStatus",
                                      Qt::QueuedConnection);
        }
    }
}

void CPropertyPushButton::noDSDataFound(const std::string& control_unit_uid,
                                        int dataset_type) {

}

void CPropertyPushButton::apiHasEndedWithError(const QString& api_tag,
                                                  QSharedPointer<chaos::CException> api_exception) {
    last_error_code = api_exception->errorCode;
    last_error_message = QString::fromStdString(api_exception->what());
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

void CPropertyPushButton::updateUIStatus() {
    //set enable according to online status
    setEnabled(online_status == OnlineStateON);

    if(last_error_code) {
        updateStateDescription(2, last_error_message);
        setButtonState(2);
    } else {
        setButtonState(last_got_value.asBool());
    }
    setChecked(last_got_value.asBool());
}

void CPropertyPushButton::on_pushButton_clicked(bool clicked) {
    last_error_code = 0;
    last_error_message.clear();
//    ChaosSharedPtr<chaos::common::data::CDataWrapperKeyValueSetter> bool_value(new chaos::common::data::CDataWrapperBoolKeyValueSetter(property_name.toStdString(),
//                                                                                                                                             clicked));
//    ChaosSharedPtr<chaos::metadata_service_client::api_proxy::node::NodePropertyGroup> cu_property_group(new chaos::metadata_service_client::api_proxy::node::NodePropertyGroup());
//    cu_property_group->group_name = property_group_name.toStdString();
//    cu_property_group->group_property_list.push_back(bool_value);

//    chaos::metadata_service_client::api_proxy::node::NodePropertyGroupList property_list;
//    property_list.push_back(cu_property_group);

    chaos::common::property::PropertyGroup pg(property_group_name.toStdString());
    pg.addProperty(property_name.toStdString(), CDataVariant(!last_got_value.asBool()));

    submitApiResult("update_property",
                    GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::UpdateProperty)->execute(nodeUID().toStdString(),
                                                                                                                pg));
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}
