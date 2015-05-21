#include "ControlUnitEditor.h"
#include "ui_controluniteditor.h"

#include <QDateTime>
#include <QDebug>
static const QString TAG_CU_INFO = QString("g_cu_i");
static const QString TAG_CU_DATASET = QString("g_cu_d");
static const QString TAG_CU_INSTANCE = QString("g_cu_instance");

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

ControlUnitEditor::ControlUnitEditor(const QString &_control_unit_unique_id) :
    PresenterWidget(NULL),
    control_unit_unique_id(_control_unit_unique_id),
    ui(new Ui::ControlUnitEditor),
    channel_output_table_model(NULL) {
    ui->setupUi(this);
    //handler connection
    connect(&status_handler,
            SIGNAL(valueUpdated(QString,QString,QVariant)),
            SLOT(updateAttributeValue(QString,QString,QVariant)));
    connect(&hb_handler,
            SIGNAL(valueUpdated(QString,QString,QVariant)),
            SLOT(updateAttributeValue(QString,QString,QVariant)));
}

ControlUnitEditor::~ControlUnitEditor() {
    if(unit_server_parent_unique_id.size()) {
        //remove old unit server for healt
        ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(unit_server_parent_unique_id.toStdString(),
                                                                                     20,
                                                                                     &status_handler);
        ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(unit_server_parent_unique_id.toStdString(),
                                                                                     20,
                                                                                     &hb_handler);
    }
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(control_unit_unique_id.toStdString(),
                                                                                 20,
                                                                                 &status_handler);
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(control_unit_unique_id.toStdString(),
                                                                                 20,
                                                                                 &hb_handler);
    delete ui;
}

void ControlUnitEditor::initUI() {
    //add model to table
    ui->tableViewOutputChannel->setModel(&channel_output_table_model);
    ui->tableViewOutputChannel->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableViewInputChannels->setModel(&channel_input_table_model);
    ui->tableViewInputChannels->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(control_unit_unique_id.toStdString(),
                                                                              20,
                                                                              &status_handler);
    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(control_unit_unique_id.toStdString(),
                                                                              20,
                                                                              &hb_handler);

    //compose logic on switch
    //logic_switch_aggregator.addNewLogicSwitch("loadeble");
    //logic_switch_aggregator.addKeyRefValue("loadeble", "us_live","true");
    //logic_switch_aggregator.addKeyRefValue("loadeble", "us_state","Load");
    //logic_switch_aggregator.addKeyRefValue("loadeble", "cu_alive","false");
    //logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("loadeble", ui->pushButtonLoadAction, "enabled", true, false);

    //launch api for control unit information
    updateAll();
}


void ControlUnitEditor::updateAll() {
    submitApiResult(QString(TAG_CU_INFO),
                    GET_CHAOS_API_PTR(node::GetNodeDescription)->execute(control_unit_unique_id.toStdString()));
    submitApiResult(QString(TAG_CU_DATASET),
                    GET_CHAOS_API_PTR(control_unit::GetCurrentDataset)->execute(control_unit_unique_id.toStdString()));
    submitApiResult(QString(TAG_CU_INSTANCE),
                    GET_CHAOS_API_PTR(control_unit::GetInstance)->execute(control_unit_unique_id.toStdString()));
}


bool ControlUnitEditor::canClose() {
    return true;
}

void ControlUnitEditor::onApiDone(const QString& tag,
                                  QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare(TAG_CU_INFO) == 0) {
        fillInfo(api_result);
    } else if(tag.compare(TAG_CU_DATASET) == 0) {
        fillDataset(api_result);
    } else if(tag.compare(TAG_CU_INSTANCE) == 0) {
        if(api_result.isNull()) return;
        if(api_result->hasKey(chaos::NodeDefinitionKey::NODE_PARENT)){
            const std::string new_u_s = api_result->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT);
            if(unit_server_parent_unique_id.compare(QString::fromStdString(new_u_s)) != 0) {
                //whe ahve unit server changed
                if(unit_server_parent_unique_id.size()) {
                    //remove old unit server for healt
                    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(unit_server_parent_unique_id.toStdString(),
                                                                                                 20,
                                                                                                 &status_handler);
                    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(unit_server_parent_unique_id.toStdString(),
                                                                                                 20,
                                                                                                 &hb_handler);
                }

                ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(new_u_s,
                                                                                          20,
                                                                                          &status_handler);
                ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(new_u_s,
                                                                                          20,
                                                                                          &hb_handler);

            }
        }
    }
}

void ControlUnitEditor::fillInfo(const QSharedPointer<chaos::common::data::CDataWrapper>& node_info) {
    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {
        ui->labelUniqueIdentifier->setText(QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)));
    }

    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
        ui->labelRemoteAddress->setText(QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR)));
    }
}

void ControlUnitEditor::updateAttributeValue(const QString& key,
                                             const QString& attribute_name,
                                             const QVariant& attribute_value) {
    if(key.startsWith(control_unit_unique_id)) {
        if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS) == 0) {
            //print the status
            if(attribute_value.isNull()) {
                ui->labelControlUnitStatus->setText(tr("-------"));
            }else{
                ui->labelControlUnitStatus->setText(attribute_value.toString());
            }
            //broadcast cu status to switch
            //logic_switch_aggregator.broadcastCurrentValueForKey("cu_state", attribute_value.toString());
        } else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0){
            //print the timestamp and update the red/green indicator
            ui->ledIndicatorHealtTSControlUnit->setNewTS(attribute_value.toULongLong());
            //broadcast cu status to switch
            //logic_switch_aggregator.broadcastCurrentValueForKey("cu_alive", (ui->ledIndicatorHealtTSControlUnit->getState()==2)?"true":"false");
        }
    } else  if(key.startsWith(unit_server_parent_unique_id)) {
        //show healt for unit server
        if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS) == 0) {
            //print the status
            if(attribute_value.isNull()) {
                ui->labelUnitServerStatus->setText(tr("-------"));
            }else{
                ui->labelUnitServerStatus->setText(attribute_value.toString());
            }
            //logic_switch_aggregator.broadcastCurrentValueForKey("us_state", attribute_value.toString());
        } else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0){
            //print the timestamp and update the red/green indicator
            ui->ledIndicatorHealtTSUnitServer->setNewTS(attribute_value.toULongLong());
            //logic_switch_aggregator.broadcastCurrentValueForKey("cu_alive", (ui->ledIndicatorHealtTSUnitServer->getState()==2)?"true":"false");
        }
    }
}

void ControlUnitEditor::onLogicSwitchChangeState(const QString& switch_name,
                              bool switch_activate) {

}

void ControlUnitEditor::fillDataset(const QSharedPointer<chaos::common::data::CDataWrapper>& dataset) {
    channel_output_table_model.updateData(dataset);
    channel_input_table_model.updateData(dataset);
}

void ControlUnitEditor::on_pushButtonLoadAction_clicked() {
    submitApiResult("cu_laod",
                    GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(control_unit_unique_id.toStdString(), true));
}

void ControlUnitEditor::on_pushButtonUnload_clicked() {
    submitApiResult("cu_unlaod",
                    GET_CHAOS_API_PTR(unit_server::LoadUnloadControlUnit)->execute(control_unit_unique_id.toStdString(), false));
}

void ControlUnitEditor::on_pushButtonInitAction_clicked() {
    submitApiResult("cu_init",
                    GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(control_unit_unique_id.toStdString(), true));
}

void ControlUnitEditor::on_pushButtonDeinitAction_clicked() {
    submitApiResult("cu_deinit",
                    GET_CHAOS_API_PTR(control_unit::InitDeinit)->execute(control_unit_unique_id.toStdString(), false));
}

void ControlUnitEditor::on_pushButtonStartAction_clicked() {
    submitApiResult("cu_start",
                    GET_CHAOS_API_PTR(control_unit::StartStop)->execute(control_unit_unique_id.toStdString(), true));
}

void ControlUnitEditor::on_pushButtonStopAction_clicked() {
    submitApiResult("cu_stop",
                    GET_CHAOS_API_PTR(control_unit::StartStop)->execute(control_unit_unique_id.toStdString(), false));
}
