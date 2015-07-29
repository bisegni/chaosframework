#include "ControlUnitEditor.h"
#include "ui_ControlUnitEditor.h"

#include "CommandTemplateInstanceEditor.h"
#include "../../widget/list/delegate/TwoLineInformationListItemDelegate.h"
#include "../../plot/NodeAttributePlotting.h"

#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QIntValidator>
#include <QObject>

static const QString TAG_CU_INFO = QString("g_cu_i");
static const QString TAG_CU_DATASET = QString("g_cu_d");
static const QString TAG_CU_INSTANCE = QString("g_cu_instance");
static const QString TAG_CU_APPLY_CHANGESET = QString("g_cu_apply_changeset");
static const QString TAG_CU_SEARCH_TEMPLATE = QString("g_cu_search_template");
static const QString TAG_CU_DELETE_TEMPLATE = QString("g_cu_delete_template");
static const QString TAG_CU_SET_THREAD_SCHEDULE_DELAY = QString("g_cu_thrd_sch_del");

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

ControlUnitEditor::ControlUnitEditor(const QString &_control_unit_unique_id) :
    PresenterWidget(NULL),
    last_online_state(false),
    control_unit_unique_id(_control_unit_unique_id),
    ui(new Ui::ControlUnitEditor),
    dataset_output_table_model(control_unit_unique_id,
                               chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT),
    dataset_input_table_model(control_unit_unique_id,
                              chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT) {
    ui->setupUi(this);
    //handler connection
    connect(ui->ledIndicatorHealtTSControlUnit,
            SIGNAL(changedOnlineStatus(QString, CLedIndicatorHealt::AliveState)),
            SLOT(changedOnlineStatus(QString, CLedIndicatorHealt::AliveState)));
    connect(ui->ledIndicatorHealtTSUnitServer,
            SIGNAL(changedOnlineStatus(QString, CLedIndicatorHealt::AliveState)),
            SLOT(changedOnlineStatus(QString, CLedIndicatorHealt::AliveState)));

}

ControlUnitEditor::~ControlUnitEditor() {
    //shutdown monitoring of channel
    dataset_input_table_model.setAttributeMonitoring(false);
    dataset_output_table_model.setAttributeMonitoring(false);
    //remove monitoring on cu and us
    manageMonitoring(false);
    delete ui;
}

void ControlUnitEditor::initUI() {
    //add model to table
    ui->tableViewOutputChannel->setModel(&dataset_output_table_model);
    ui->tableViewOutputChannel->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableViewInputChannels->setModel(&dataset_input_table_model);
    ui->tableViewInputChannels->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //compose logic on switch
    //for load button
    logic_switch_aggregator.addNewLogicSwitch("cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("cu_can_operate", "us_alive","online");
    logic_switch_aggregator.addKeyRefValue("cu_can_operate", "us_state","Load");

    logic_switch_aggregator.addNewLogicSwitch("load");
    logic_switch_aggregator.connectSwitch("load", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("load", "cu_alive","offline");
    logic_switch_aggregator.addKeyRefValue("load", "cu_alive","not_found");
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("load", ui->pushButtonLoadAction, "enabled", true, false);

    logic_switch_aggregator.addNewLogicSwitch("unload");
    logic_switch_aggregator.connectSwitch("unload", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("unload", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("unload", "cu_state","Load");
    logic_switch_aggregator.addKeyRefValue("unload", "cu_state","Deinit");
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("unload", ui->pushButtonUnload, "enabled", true, false);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("unload", ui->pushButtonInitAction, "enabled", true, false);

    logic_switch_aggregator.addNewLogicSwitch("start");
    logic_switch_aggregator.connectSwitch("start", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("start", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("start", "cu_state","Init");
    logic_switch_aggregator.addKeyRefValue("start", "cu_state","Stop");
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("start", ui->pushButtonStartAction, "enabled", true, false);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("start", ui->pushButtonDeinitAction, "enabled", true, false);

    logic_switch_aggregator.addNewLogicSwitch("stop");
    logic_switch_aggregator.connectSwitch("stop", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("stop", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("stop", "cu_state","Start");
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("stop", ui->pushButtonStopAction, "enabled", true, false);


    logic_switch_aggregator.addNewLogicSwitch("update_property");
    logic_switch_aggregator.connectSwitch("update_property", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("update_property", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("update_property", "cu_state","Init");
    logic_switch_aggregator.addKeyRefValue("update_property", "cu_state","Start");
    logic_switch_aggregator.addKeyRefValue("update_property", "cu_state","Stop");
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("update_property", ui->pushButtonSetRunScheduleDelay, "enabled", true, false);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("update_property", ui->lineEditRunScheduleDelay, "enabled", true, false);

    //set the status on not_found for either the control unit and unit serve
    logic_switch_aggregator.broadcastCurrentValueForKey("cu_alive", getStatusString(0));
    logic_switch_aggregator.broadcastCurrentValueForKey("us_alive", getStatusString(0));

    //set control unit uid label
    ui->labelUniqueIdentifier->setText(control_unit_unique_id);

    //command section
    QList<int> splitterCLSizes;
    splitterCLSizes.append(300);
    splitterCLSizes.append(600);
    ui->splitterCommandList->setSizes(splitterCLSizes);
    ui->splitterCommandList->setStretchFactor(0,1);
    ui->splitterCommandList->setStretchFactor(1,2);
    ui->listViewCommandList->setModel(&command_list_model);
    ui->listViewCommandList->setItemDelegate(new TwoLineInformationListItemDelegate(ui->listViewCommandList));
    connect(ui->listViewCommandList->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleSelectionChangedOnListWiew(QItemSelection,QItemSelection)));

    //command template
    ui->listViewCommandInstance->setModel(&command_template_list_model);
    connect(ui->listViewCommandInstance->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleSelectionChangedOnListWiew(QItemSelection,QItemSelection)));


    //led indicator
    ui->ledIndicatorHealtTSControlUnit->setStateBlinkOnRepeatSet(2, true);
    ui->ledIndicatorHealtTSUnitServer->setStateBlinkOnRepeatSet(2, true);

    //setup chaos widget
    //cu helat indicator
    ui->ledIndicatorHealtTSControlUnit->setNodeUniqueID(control_unit_unique_id);

    //control unit status
    ui->labelControlUnitStatus->setNodeUniqueID(control_unit_unique_id);
    ui->labelControlUnitStatus->setTrackStatus(true);
    ui->labelControlUnitStatus->setLabelValueShowTrackStatus(true);
    connect(ui->labelControlUnitStatus,
            SIGNAL(valueChanged(QString,QString)),
            SLOT(changedNodeState(QString,QString)));

    //unit server status
    ui->labelUnitServerStatus->setTrackStatus(true);
    ui->labelUnitServerStatus->setLabelValueShowTrackStatus(true);
    connect(ui->labelUnitServerStatus,
            SIGNAL(valueChanged(QString,QString)),
            SLOT(changedNodeState(QString,QString)));

    //show the current thread schedule delay
    ui->labelRunScheduleDelaySet->setNodeUniqueID(control_unit_unique_id);
    ui->labelRunScheduleDelaySet->setAttributeName(chaos::ControlUnitNodeDefinitionKey::THREAD_SCHEDULE_DELAY);
    ui->labelRunScheduleDelaySet->setAttributeType(chaos::DataType::TYPE_INT64);
    ui->labelRunScheduleDelaySet->setTrackStatus(true);
    ui->labelRunScheduleDelaySet->setDataset(ChaosDatasetLabel::DatasetSystem);

    //thread schedule update
    ui->lineEditRunScheduleDelay->setValidator(new QIntValidator(0,60000000));
    // ui->listWidgetCommandList->setItemDelegate(new CommandItemDelegate(ui->listWidgetCommandList));

    //start monitoring
    manageMonitoring(true);

    //launch api for control unit information
    updateAllControlUnitInfomration();
}

void ControlUnitEditor::updateTemplateSearch() {
    node::CommandUIDList uid_filter;
    if(ui->listViewCommandList->selectionModel()->selectedRows().size()) {
        foreach(QModelIndex row, ui->listViewCommandList->selectionModel()->selectedRows()) {
            QVariant user_data = row.data(Qt::UserRole);
            if(user_data.canConvert<QString>()) {
                uid_filter.push_back(user_data.toString().toStdString());
            }
        }
    }else{
        command_list_model.fillWithCommandUID(uid_filter);
    }

    if(uid_filter.size()!=0) {
        submitApiResult(QString(TAG_CU_SEARCH_TEMPLATE),
                        GET_CHAOS_API_PTR(node::CommandTemplateSearch)->execute(uid_filter));
    }
}

void ControlUnitEditor::updateAllControlUnitInfomration() {
    submitApiResult(QString(TAG_CU_INFO),
                    GET_CHAOS_API_PTR(node::GetNodeDescription)->execute(control_unit_unique_id.toStdString()));
    submitApiResult(QString(TAG_CU_DATASET),
                    GET_CHAOS_API_PTR(control_unit::GetCurrentDataset)->execute(control_unit_unique_id.toStdString()));
}

void ControlUnitEditor::manageMonitoring(bool start) {
    if(start){
        ui->labelControlUnitStatus->startMonitoring();
        ui->labelRunScheduleDelaySet->startMonitoring();
        ui->ledIndicatorHealtTSControlUnit->startMonitoring();
    }else{
        if(unit_server_parent_unique_id.size()) {
            //remove old unit server for healt
            ui->labelUnitServerStatus->stopMonitoring();
            ui->ledIndicatorHealtTSUnitServer->stopMonitoring();
        }
        ui->labelControlUnitStatus->stopMonitoring();
        ui->labelRunScheduleDelaySet->stopMonitoring();
        ui->ledIndicatorHealtTSControlUnit->stopMonitoring();
    }
}


void ControlUnitEditor::changedOnlineStatus(const QString& node_uid,
                                            CLedIndicatorHealt::AliveState node_alive_state) {
    if(node_uid.compare(control_unit_unique_id) == 0) {
        if(node_alive_state == CLedIndicatorHealt::Online) {
            //Control unit has becomed online so we need to reload all information
            updateAllControlUnitInfomration();
        }
        //state changed for control unit
        qDebug()<< "change cu online status for:" << node_uid << " as:" <<getStatusString(ui->ledIndicatorHealtTSControlUnit->getState());
        logic_switch_aggregator.broadcastCurrentValueForKey("cu_alive", getStatusString(ui->ledIndicatorHealtTSControlUnit->getState()));
    } else if(node_uid.compare(unit_server_parent_unique_id) == 0) {
        //state changed for unit server
        qDebug()<< "change us online status for:" << node_uid << " as:" <<getStatusString(ui->ledIndicatorHealtTSUnitServer->getState());
        logic_switch_aggregator.broadcastCurrentValueForKey("us_alive", getStatusString(ui->ledIndicatorHealtTSUnitServer->getState()));
    }
}

void ControlUnitEditor::changedNodeState(const QString& node_uid,
                                         const QString& value) {
    if(node_uid.compare(control_unit_unique_id) == 0) {
        //state changed for control unit
        qDebug()<< "change cu state for:" << node_uid << " as:" <<value;
        logic_switch_aggregator.broadcastCurrentValueForKey("cu_state", value);
    } else if(node_uid.compare(unit_server_parent_unique_id) == 0) {
        //state changed for unit server
        qDebug()<< "change us state for:" << node_uid << " as:" <<value;
        logic_switch_aggregator.broadcastCurrentValueForKey("us_state", value);
    }
}

bool ControlUnitEditor::canClose() {
    return true;
}

QString ControlUnitEditor::getStatusString(int status) {
    switch(status) {
    case 0:
        return QString("not_found");
        break;
    case 1:
        return QString("offline");
        break;
    case 2:
        return QString("online");
        break;
    }
}

void ControlUnitEditor::onApiDone(const QString& tag,
                                  QSharedPointer<CDataWrapper> api_result) {
    if(tag.compare(TAG_CU_INFO) == 0) {
        fillInfo(api_result);
    } else if(tag.compare(TAG_CU_DATASET) == 0) {
        fillDataset(api_result);
        submitApiResult(QString(TAG_CU_INSTANCE),
                        GET_CHAOS_API_PTR(control_unit::GetInstance)->execute(control_unit_unique_id.toStdString()));
    } else if(tag.compare(TAG_CU_INSTANCE) == 0) {
        if(api_result.isNull()) return;
        if(api_result->hasKey(chaos::NodeDefinitionKey::NODE_PARENT)){
            const QString new_u_s = QString::fromStdString(api_result->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT));
            if(unit_server_parent_unique_id.compare(new_u_s) != 0) {
                //whe ahve unit server changed
                if(unit_server_parent_unique_id.size()) {
                    //remove old unit server for healt
                    ui->labelUnitServerStatus->stopMonitoring();
                    ui->ledIndicatorHealtTSUnitServer->stopMonitoring();
                }
                unit_server_parent_unique_id = new_u_s;

                ui->labelUnitServerStatus->setNodeUniqueID(unit_server_parent_unique_id);
                ui->labelUnitServerStatus->startMonitoring();

                ui->ledIndicatorHealtTSUnitServer->setNodeUniqueID(unit_server_parent_unique_id);
                ui->ledIndicatorHealtTSUnitServer->startMonitoring();
                // keep track of new us uid
                ui->labelUnitServerUID->setText(unit_server_parent_unique_id);
            }
            //apply control unit instance
            dataset_input_table_model.updateInstanceDescription(api_result);
        }
    } else if(tag.compare(TAG_CU_APPLY_CHANGESET) == 0) {
        dataset_input_table_model.applyChangeSet(true);
    } else if(tag.compare(TAG_CU_SEARCH_TEMPLATE) == 0) {
        command_template_list_model.updateSearchPage(api_result);
    } else if(tag.compare(TAG_CU_DELETE_TEMPLATE) == 0) {
        //tempalte delete operaion
        updateTemplateSearch();
    }
}

void ControlUnitEditor::fillInfo(const QSharedPointer<chaos::common::data::CDataWrapper>& node_info) {
    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
        ui->labelRemoteAddress->setText(QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR)));
    } else {
        ui->labelRemoteAddress->setText(tr(""));
    }

    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)) {
        ui->labelRpcDomain->setText(QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)));
    } else {
        ui->labelRpcDomain->setText(tr(""));
    }
}

void ControlUnitEditor::monitorHandlerUpdateAttributeValue(const QString& key,
                                                           const QString& attribute_name,
                                                           const QVariant& attribute_value) {
    if(key.startsWith(getHealttKeyFromNodeKey(control_unit_unique_id))) {
        if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS) == 0) {
            //print the status
            if(attribute_value.isNull()) {
                ui->labelControlUnitStatus->setText(tr(""));
            }else{
                ui->labelControlUnitStatus->setText(attribute_value.toString());
            }
            //broadcast cu status to switch
            logic_switch_aggregator.broadcastCurrentValueForKey("cu_state", attribute_value.toString());
            if( attribute_value.toString().compare(tr("Load"))==0){
                bool current_relevated_online_status = ui->ledIndicatorHealtTSControlUnit->getState()==2;
                if(current_relevated_online_status != last_online_state) {
                    if(current_relevated_online_status) {
                        //we need to reload all information
                        updateAllControlUnitInfomration();
                    }
                    last_online_state = current_relevated_online_status;
                }
            }
        } else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0){
            //print the timestamp and update the red/green indicator
            //ui->ledIndicatorHealtTSControlUnit->setNewTS(attribute_value.toULongLong());
            ////broadcast cu status to switch
            logic_switch_aggregator.broadcastCurrentValueForKey("cu_alive", getStatusString(ui->ledIndicatorHealtTSControlUnit->getState()));
        }
    } else if(key.startsWith(unit_server_parent_unique_id)) {
        //show healt for unit server
        if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS) == 0) {
            //print the status
            if(attribute_value.isNull()) {
                ui->labelUnitServerStatus->setText(tr(""));
            }else{
                ui->labelUnitServerStatus->setText(attribute_value.toString());
            }
            logic_switch_aggregator.broadcastCurrentValueForKey("us_state", attribute_value.toString());
        } else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0){
            //print the timestamp and update the red/green indicator
            // ui->ledIndicatorHealtTSUnitServer->setNewTS(attribute_value.toULongLong());
            logic_switch_aggregator.broadcastCurrentValueForKey("us_alive", getStatusString(ui->ledIndicatorHealtTSUnitServer->getState()));
        }
    }
}

void ControlUnitEditor::templateSaved(const QString& tempalte_name,
                                      const QString& command_uid) {
    //after temaplte has been saved update the search
    updateTemplateSearch();
}

void ControlUnitEditor::onLogicSwitchChangeState(const QString& switch_name,
                                                 bool switch_activate) {

}

void ControlUnitEditor::handleSelectionChangedOnListWiew(const QItemSelection& selection,
                                                         const QItemSelection &previous_selected) {
    QObject* sender = QObject::sender()->parent();
    if(ui->listViewCommandList == sender) {
        ui->pushButtonAddNewCommadInstance->setEnabled(ui->listViewCommandList->selectionModel()->selectedRows().size()==1);
    }else if(ui->listViewCommandInstance == sender){
        unsigned int selected_template_count = ui->listViewCommandInstance->selectionModel()->selectedRows().size();
        ui->pushButtonEditInstance->setEnabled(selected_template_count);
        ui->pushButtonRemoveInstance->setEnabled(selected_template_count == 1);
        ui->pushButtonCreateInstance->setEnabled(selected_template_count == 1);
    }
}

void ControlUnitEditor::fillDataset(const QSharedPointer<chaos::common::data::CDataWrapper>& dataset) {
    dataset_output_table_model.updateData(dataset);
    dataset_input_table_model.updateData(dataset);
    command_list_model.updateData(dataset);
    //update search on tempalte list()
    updateTemplateSearch();
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

void ControlUnitEditor::on_checkBoxMonitorOutputChannels_clicked() {
    dataset_output_table_model.setAttributeMonitoring(ui->checkBoxMonitorOutputChannels->isChecked());
}

void ControlUnitEditor::on_checkBoxMonitorInputChannels_clicked() {
    dataset_input_table_model.setAttributeMonitoring(ui->checkBoxMonitorInputChannels->isChecked());
}

void ControlUnitEditor::on_pushButtonCommitSet_clicked() {
    std::vector< boost::shared_ptr< control_unit::ControlUnitInputDatasetChangeSet > > value_set_array;
    dataset_input_table_model.getAttributeChangeSet(value_set_array);
    if(value_set_array[0]->change_set.size()==0) {
        QMessageBox msgBox;
        msgBox.setText(tr("Error applying changeset."));
        msgBox.setInformativeText(tr("No updated input attribute has been found!."));
        msgBox.exec();
        return;
    }
    submitApiResult(TAG_CU_APPLY_CHANGESET,
                    GET_CHAOS_API_PTR(control_unit::SetInputDatasetAttributeValues)->execute(value_set_array));
}

void ControlUnitEditor::on_pushButtonResetChangeSet_clicked() {
    dataset_input_table_model.applyChangeSet(false);
}

void ControlUnitEditor::on_pushButtonAddNewCommadInstance_clicked() {
    QModelIndexList selected_list = ui->listViewCommandList->selectionModel()->selectedRows();
    if(selected_list.size()!=1) return;

    //! open tempalte editor for new instance creation
    QSharedPointer<TwoLineInformationItem> item = selected_list.first().data().value< QSharedPointer<TwoLineInformationItem> >();
    ControlUnitCommandTemplateEditor    *template_editor= new ControlUnitCommandTemplateEditor(QString::fromStdString(item->raw_data->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)));
    //connect tempalte editor
    connect(template_editor,
            SIGNAL(templateSaved(QString,QString)),
            SLOT(templateSaved(QString,QString)));

    addWidgetToPresenter(template_editor);
}

void ControlUnitEditor::on_pushButtonUpdateTemaplteList_clicked() {
    updateTemplateSearch();
}

void ControlUnitEditor::on_pushButtonEditInstance_clicked() {
    //scann all selected element and open the editor for each one
    foreach (QModelIndex index, ui->listViewCommandInstance->selectionModel()->selectedRows()) {
        //! open tempalte editor for new instance creation
        ControlUnitCommandTemplateEditor    *template_editor= new ControlUnitCommandTemplateEditor(index.data().toString(),
                                                                                                   index.data(Qt::UserRole).toString());
        //connect tempalte editor
        connect(template_editor,
                SIGNAL(templateSaved(QString,QString)),
                SLOT(templateSaved(QString,QString)));

        addWidgetToPresenter(template_editor);
    }
}

void ControlUnitEditor::on_pushButtonRemoveInstance_clicked() {
    foreach(QModelIndex index, ui->listViewCommandInstance->selectionModel()->selectedRows()) {
        //invoke api for template delete operation
        submitApiResult(TAG_CU_DELETE_TEMPLATE,
                        GET_CHAOS_API_PTR(node::CommandTemplateDelete)->execute(index.data().toString().toStdString(),
                                                                                index.data(Qt::UserRole).toString().toStdString()));
    }
}

void ControlUnitEditor::on_pushButtonCreateInstance_clicked() {
    foreach(QModelIndex index,  ui->listViewCommandInstance->selectionModel()->selectedRows()) {
        addWidgetToPresenter(new CommandTemplateInstanceEditor(control_unit_unique_id,
                                                               index.data().toString(),
                                                               index.data(Qt::UserRole).toString()));
    }
}

void ControlUnitEditor::on_pushButtonSetRunScheduleDelay_clicked() {
    chaos::metadata_service_client::api_proxy::node::NodePropertyGroupList property_list;
    boost::shared_ptr<chaos::common::data::CDataWrapperKeyValueSetter> thread_run_schedule(new chaos::common::data::CDataWrapperInt64KeyValueSetter(chaos::ControlUnitNodeDefinitionKey::THREAD_SCHEDULE_DELAY,
                                                                                                                                                    ui->lineEditRunScheduleDelay->text().toLongLong()));
    boost::shared_ptr<chaos::metadata_service_client::api_proxy::node::NodePropertyGroup> cu_property_group(new chaos::metadata_service_client::api_proxy::node::NodePropertyGroup());
    cu_property_group->group_name = "property_abstract_control_unit";
    cu_property_group->group_property_list.push_back(thread_run_schedule);

    property_list.push_back(cu_property_group);

    submitApiResult(TAG_CU_SET_THREAD_SCHEDULE_DELAY,
                    GET_CHAOS_API_PTR(node::UpdateProperty)->execute(control_unit_unique_id.toStdString(),
                                                                     property_list));
}

void ControlUnitEditor::on_pushButton_clicked() {
    NodeAttributePlotting *plot_viewer = new NodeAttributePlotting(control_unit_unique_id, NULL);
    plot_viewer->show();
}
