#include "CPropertyTextEdit.h"
#include "ui_CPropertyTextEdit.h"

using namespace chaos::common::data;
using namespace chaos::common::property;
using namespace chaos::metadata_service_client::node_monitor;

CPropertyTextEdit::CPropertyTextEdit(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this),
    ui(new Ui::CPropertyTextEdit) {
    ui->setupUi(this);

    ui->pushButtonSavePropertyValue->addState(CStateVisiblePushButton::StateInfo("Disabled", Qt::lightGray));
    ui->pushButtonSavePropertyValue->addState(CStateVisiblePushButton::StateInfo("Enable", Qt::green));
    ui->pushButtonSavePropertyValue->addState(CStateVisiblePushButton::StateInfo("In Error", Qt::red));
    ui->pushButtonSavePropertyValue->setCheckable(false);

    ui->pushButtonSavePropertyValue->setFocus();
}

CPropertyTextEdit::~CPropertyTextEdit() {
    deinitChaosContent();
    delete ui;
}

void CPropertyTextEdit::initChaosContent() {
    trackNode();
}
void CPropertyTextEdit::deinitChaosContent() {
    untrackNode();
}
void CPropertyTextEdit::updateChaosContent() {

}

void CPropertyTextEdit::setPropertyGroup(const QString& new_property_group) {
    property_group = new_property_group;
}

const QString& CPropertyTextEdit::propertyGroup() {
    return property_group;
}

void CPropertyTextEdit::setPropertyName(const QString& new_property_name) {
    property_name = new_property_name;
}

const QString& CPropertyTextEdit::propertyName() {
    return property_name;
}

void CPropertyTextEdit::nodeChangedOnlineState(const std::string& node_uid,
                                               chaos::metadata_service_client::node_monitor::OnlineState old_status,
                                               chaos::metadata_service_client::node_monitor::OnlineState new_status) {
    online_status = new_status;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

void CPropertyTextEdit::updatedDS(const std::string& control_unit_uid,
                                  int dataset_type,
                                  chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values) {
    if(dataset_type == DatasetTypeSystem) {
        if(dataset_key_values.count(property_name.toStdString())) {
            current_property_value = QString::fromStdString(dataset_key_values[property_name.toStdString()].asString());
            QMetaObject::invokeMethod(this,
                                      "updateUIStatus",
                                      Qt::QueuedConnection);
        }
    }
}

void CPropertyTextEdit::noDSDataFound(const std::string& control_unit_uid,
                                      int dataset_type) {
    current_property_value = "---";
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}

void CPropertyTextEdit::updateUIStatus() {
    ui->lineEditPropertyValue->setEnabled(online_status == OnlineStateON);
    ui->pushButtonSavePropertyValue->setEnabled(online_status == OnlineStateON);
    if(online_status == OnlineStateON) {
        if(last_error_message.size()) {
            ui->pushButtonSavePropertyValue->updateStateDescription(2, last_error_message);
            ui->pushButtonSavePropertyValue->setButtonState(2);
        } else {
            ui->pushButtonSavePropertyValue->setButtonState(1);
        }
    }else{
        ui->pushButtonSavePropertyValue->setButtonState(0);
    }

    ui->lineEditPropertyValue->setPlaceholderText(QString("[%1] %2").arg(property_name).arg(current_property_value));
}

void CPropertyTextEdit::on_pushButtonSavePropertyValue_clicked() {
    last_error_message.clear();
    PropertyGroup pg(property_group.toStdString());
    pg.addProperty(property_name.toStdString(), CDataVariant(ui->lineEditPropertyValue->text().toStdString()));
    ui->lineEditPropertyValue->clear();
    submitApiResult("update_property",
                    GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::UpdateProperty)->execute(nodeUID().toStdString(),
                                                                                                                pg));
}

void CPropertyTextEdit::apiHasStarted(const QString& api_tag) {
    setEnabled(false);
}

void CPropertyTextEdit::apiHasEnded(const QString& api_tag) {
    setEnabled(true);
}

void CPropertyTextEdit::apiHasEndedWithError(const QString& api_tag,
                                             QSharedPointer<chaos::CException> api_exception) {
    setEnabled(true);
    last_error_message = QString::fromStdString(api_exception->what());
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);
}
