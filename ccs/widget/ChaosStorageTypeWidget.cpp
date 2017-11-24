#include "ChaosStorageTypeWidget.h"
#include "CPropertyTextEdit.h"
#include "CPopupWidgetContainer.h"
#include "ui_ChaosStorageTypeWidget.h"
#include <QDebug>

using namespace chaos::common::data;
using namespace chaos::common::property;

using namespace chaos::DataServiceNodeDefinitionType;
using namespace chaos::metadata_service_client::node_monitor;

ChaosStorageTypeWidget::ChaosStorageTypeWidget(QWidget *parent) :
    QWidget(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this),
    ui(new Ui::ChaosStorageTypeWidget),
    data_found(false),
    last_pushbutton_in_error(),
    last_error_message(){
    ui->setupUi(this);

    //configure state on push button
    ui->pushButtonUndefined->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonUndefined->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonUndefined->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    ui->pushButtonUndefined->setCheckable(true);
    ui->pushButtonUndefined->setObjectName("DSStorageTypeUndefined");
    ui->pushButtonUndefined->setProperty("storage_code", DSStorageTypeUndefined);
    connect(ui->pushButtonUndefined,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));
    ui->pushButtonHistory->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonHistory->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonHistory->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    ui->pushButtonHistory->setCheckable(true);
    ui->pushButtonHistory->setObjectName("DSStorageTypeHistory");
    ui->pushButtonHistory->setProperty("storage_code", DSStorageTypeHistory);
    connect(ui->pushButtonHistory,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));

    ui->pushButtonLive->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonLive->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonLive->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    ui->pushButtonLive->setCheckable(true);
    ui->pushButtonLive->setObjectName("DSStorageTypeLive");
    ui->pushButtonLive->setProperty("storage_code", DSStorageTypeLive);
    connect(ui->pushButtonLive,
            SIGNAL(clicked(bool)),
            SLOT(on_pushButton_clicked(bool)));

    ui->pushButtonLiveAndHistory->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonLiveAndHistory->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonLiveAndHistory->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    ui->pushButtonLiveAndHistory->setCheckable(true);
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
    online_status = new_status;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
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

#define IS_IN_ERROR(p, e)\
   ( p->objectName().compare(e) == 0)

void ChaosStorageTypeWidget::updateUIStatus() {
    //set enable according to online status
    setEnabled(online_status == OnlineStateON);

    //take on off ccording to storage type
    if(IS_IN_ERROR(ui->pushButtonLive, last_pushbutton_in_error)) {
        ui->pushButtonLive->updateStateDescription(2, last_error_message);
        ui->pushButtonLive->setButtonState(2);
    } else {
        ui->pushButtonLive->setButtonState(storage_type == DSStorageTypeLive);
    }
    ui->pushButtonLive->setChecked(storage_type == DSStorageTypeLive);

    if(IS_IN_ERROR(ui->pushButtonHistory, last_pushbutton_in_error)) {
        ui->pushButtonHistory->updateStateDescription(2, last_error_message);
        ui->pushButtonHistory->setButtonState(2);
    } else {
        ui->pushButtonHistory->setButtonState(storage_type == DSStorageTypeHistory);
    }
    ui->pushButtonHistory->setChecked(storage_type == DSStorageTypeHistory);

    if(IS_IN_ERROR(ui->pushButtonLiveAndHistory, last_pushbutton_in_error)) {
        ui->pushButtonLiveAndHistory->updateStateDescription(2, last_error_message);
        ui->pushButtonLiveAndHistory->setButtonState(2);
    } else {
        ui->pushButtonLiveAndHistory->setButtonState(storage_type == DSStorageTypeLiveHistory);
    }
    ui->pushButtonLiveAndHistory->setChecked(storage_type == DSStorageTypeLiveHistory);

    if(IS_IN_ERROR(ui->pushButtonUndefined, last_pushbutton_in_error)) {
        ui->pushButtonUndefined->updateStateDescription(2, last_error_message);
        ui->pushButtonUndefined->setButtonState(2);
    } else {
        ui->pushButtonUndefined->setButtonState(storage_type == DSStorageTypeUndefined);
    }
    ui->pushButtonUndefined->setChecked(storage_type == DSStorageTypeUndefined);
}

void ChaosStorageTypeWidget::on_pushButton_clicked(bool clicked) {
    CStateVisiblePushButton* obj = (CStateVisiblePushButton*)sender();
    //if we have a button in error we need to reset it
    last_pushbutton_in_error.clear();
    last_error_message.clear();
    sendStorageType((chaos::DataServiceNodeDefinitionType::DSStorageType)obj->property("storage_code").toInt(),
                    obj->objectName());
}

void ChaosStorageTypeWidget::sendStorageType(chaos::DataServiceNodeDefinitionType::DSStorageType type,
                                             const QString& event_tag) {
    PropertyGroup pg(chaos::ControlUnitPropertyKey::GROUP_NAME);
    pg.addProperty(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_TYPE, CDataVariant(type));

    submitApiResult(event_tag,
                    GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::UpdateProperty)->execute(nodeUID().toStdString(),
                                                                                                                pg));
}

void ChaosStorageTypeWidget::apiHasStarted(const QString& api_tag) {}

void ChaosStorageTypeWidget::apiHasEnded(const QString& api_tag) {}

void ChaosStorageTypeWidget::apiHasEndedWithError(const QString& api_tag,
                                                  QSharedPointer<chaos::CException> api_exception) {
    last_pushbutton_in_error = api_tag;
    last_error_message = QString::fromStdString(api_exception->what());
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

void ChaosStorageTypeWidget::on_pushButtonEdit_clicked() {
    CPropertyTextEdit *pte_live_time = new  CPropertyTextEdit(NULL);
    pte_live_time->setNodeUID(nodeUID());
    pte_live_time->setPropertyGroup(chaos::ControlUnitPropertyKey::GROUP_NAME);
    pte_live_time->setPropertyName(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME);
    pte_live_time->initChaosContent();

    CPropertyTextEdit *pte_history_time = new  CPropertyTextEdit(NULL);
    pte_history_time->setNodeUID(nodeUID());
    pte_history_time->setPropertyGroup(chaos::ControlUnitPropertyKey::GROUP_NAME);
    pte_history_time->setPropertyName(chaos::DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME);
    pte_history_time->initChaosContent();

    CPopupWidgetContainer *wc = new CPopupWidgetContainer(this);
    wc->addWidget(pte_live_time);
    wc->addWidget(pte_history_time);
    QRect rect = QRect(0,0,parentWidget()->width(),0);
    wc->setGeometry(rect);
    wc->setWindowModality(Qt::WindowModal);
    wc->show();
}
