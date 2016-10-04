#include "ChaosStorageTypeWidget.h"
#include "ui_ChaosStorageTypeWidget.h"
#include <QDebug>

using namespace chaos::DataServiceNodeDefinitionType;
using namespace chaos::metadata_service_client::node_monitor;

ChaosStorageTypeWidget::ChaosStorageTypeWidget(QWidget *parent) :
    QWidget(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this),
    ui(new Ui::ChaosStorageTypeWidget),
    data_found(false),
    last_pushbutton_successfull(){
    ui->setupUi(this);

    //configure state on push button
    ui->pushButtonUndefined->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonUndefined->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonUndefined->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    //ui->pushButtonUndefined->setCheckable(true);
    ui->pushButtonUndefined->setObjectName("DSStorageTypeUndefined");
    ui->pushButtonUndefined->setProperty("storage_code", DSStorageTypeUndefined);
    connect(ui->pushButtonUndefined,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));
    ui->pushButtonHistory->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonHistory->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonHistory->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    //ui->pushButtonHistory->setCheckable(true);
    ui->pushButtonHistory->setObjectName("DSStorageTypeHistory");
    ui->pushButtonHistory->setProperty("storage_code", DSStorageTypeHistory);
    connect(ui->pushButtonHistory,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));

    ui->pushButtonLive->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonLive->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonLive->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    //ui->pushButtonLive->setCheckable(true);
    ui->pushButtonLive->setObjectName("DSStorageTypeLive");
    ui->pushButtonLive->setProperty("storage_code", DSStorageTypeLive);
    connect(ui->pushButtonLive,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));

    ui->pushButtonLiveAndHistory->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonLiveAndHistory->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonLiveAndHistory->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    //ui->pushButtonLiveAndHistory->setCheckable(true);
    ui->pushButtonLiveAndHistory->setObjectName("DSStorageTypeLiveHistory");
    ui->pushButtonLiveAndHistory->setProperty("storage_code", DSStorageTypeLiveHistory);
    connect(ui->pushButtonLiveAndHistory,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));

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
    if(dataset_type == DatasetTypeSystem) {
        if(dataset_key_values.count(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE)) {
            storage_type = static_cast<chaos::DataServiceNodeDefinitionType::DSStorageType>(dataset_key_values[chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE].asInt32());
        } else {
            storage_type = DSStorageTypeUndefined;
        }
    }
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

void ChaosStorageTypeWidget::noDSDataFound(const std::string& control_unit_uid,
                                           int dataset_type) {
    data_found = false;
    storage_type = DSStorageTypeUndefined;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
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

void ChaosStorageTypeWidget::updateButtonCheckedStatus(QObject *target, bool checked) {
    CStateVisiblePushButton *_target = (CStateVisiblePushButton*)target;
    _target->setChecked(checked);
}

void ChaosStorageTypeWidget::updateButtonSetErrorStatus(QObject *target, const QString& error_message) {
    CStateVisiblePushButton *_target = (CStateVisiblePushButton*)target;
    _target->updateStateDescription(2, error_message);
    _target->setButtonState(2);
}

void ChaosStorageTypeWidget::resetButtonInError(QObject *target) {
    CStateVisiblePushButton *_target = (CStateVisiblePushButton*)target;
    _target->setButtonState(0);
}

void ChaosStorageTypeWidget::on_pushButton_clicked(bool clicked) {
    CStateVisiblePushButton* obj = (CStateVisiblePushButton*)sender();

    //if we have a button in error we need to reset it
    if(last_pushbutton_successfull.size()) {
        QObject *button_in_success = findChild<QObject*>(last_pushbutton_successfull);
       /* QMetaObject::invokeMethod(this,
                                  "updateButtonCheckedStatus",
                                  Qt::QueuedConnection,
                                  Q_ARG(QObject*, button_in_success),
                                  Q_ARG(bool, false));*/
        last_pushbutton_successfull.clear();
    }
    sendStorageType((chaos::DataServiceNodeDefinitionType::DSStorageType)obj->property("storage_code").toInt(),
                    obj->objectName());
}

void ChaosStorageTypeWidget::sendStorageType(chaos::DataServiceNodeDefinitionType::DSStorageType type,
                                             const QString& event_tag) {
    chaos::metadata_service_client::api_proxy::node::NodePropertyGroupList property_list;
    boost::shared_ptr<chaos::common::data::CDataWrapperKeyValueSetter> storage_type(new chaos::common::data::CDataWrapperInt32KeyValueSetter(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE,
                                                                                                                                             type));
    boost::shared_ptr<chaos::metadata_service_client::api_proxy::node::NodePropertyGroup> cu_property_group(new chaos::metadata_service_client::api_proxy::node::NodePropertyGroup());
    cu_property_group->group_name = "property_abstract_control_unit";
    cu_property_group->group_property_list.push_back(storage_type);

    property_list.push_back(cu_property_group);

    submitApiResult(event_tag,
                    GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::UpdateProperty)->execute(nodeUID().toStdString(),
                                                                                                                property_list));
}

void ChaosStorageTypeWidget::apiHasStarted(const QString& api_tag) {
    QObject *sender = findChild<QObject*>(api_tag);
}

void ChaosStorageTypeWidget::apiHasEnded(const QString& api_tag) {
    QObject *sender = findChild<QObject*>(last_pushbutton_successfull = api_tag);
}

void ChaosStorageTypeWidget::apiHasEndedWithError(const QString& api_tag,
                                        QSharedPointer<chaos::CException> api_exception) {
    QObject *sender = findChild<QObject*>(api_tag);
    QMetaObject::invokeMethod(this,
                              "updateButtonSetErrorStatus",
                              Qt::QueuedConnection,
                              Q_ARG(QObject*, sender),
                              Q_ARG(const QString&, QString::fromStdString(api_exception->what())));
    /*QMetaObject::invokeMethod(this,
                              "updateButtonCheckedStatus",
                              Qt::QueuedConnection,
                              Q_ARG(QObject*, sender),
                              Q_ARG(bool, false));*/
}
