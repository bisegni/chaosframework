#include "ControlUnitEditor.h"
#include "ui_ControlUnitEditor.h"
#include "ControUnitInstanceEditor.h"
#include "CommandTemplateInstanceEditor.h"
#include "../../data/delegate/TwoLineInformationListItemDelegate.h"
#include "../../plot/NodeAttributePlotting.h"
#include "../../metatypes.h"

#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QIntValidator>
#include <QObject>

static const QString TAG_CU_INFO = QString("g_cu_i");
static const QString TAG_CU_INFO_FOR_UPDATE = QString("g_cu_i_update");
static const QString TAG_CU_DATASET = QString("g_cu_d");
static const QString TAG_CU_DATASET_FOR_UPDATE = QString("g_cu_d_update");
static const QString TAG_CU_INSTANCE = QString("g_cu_instance");
static const QString TAG_CU_APPLY_CHANGESET = QString("g_cu_apply_changeset");
static const QString TAG_CU_SEARCH_TEMPLATE = QString("g_cu_search_template");
static const QString TAG_CU_DELETE_TEMPLATE = QString("g_cu_delete_template");
static const QString TAG_CU_SET_THREAD_SCHEDULE_DELAY = QString("g_cu_thrd_sch_del");
static const QString TAG_CU_RECOVER_ERROR = QString("g_cu_rec_err");

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::node_monitor;

ControlUnitEditor::ControlUnitEditor(const QString &_control_unit_unique_id) :
    PresenterWidget(NULL),
    ui(new Ui::ControlUnitEditor),
    restarted(false),
    last_online_state(false),
    control_unit_unique_id(_control_unit_unique_id),
    alarm_list_model(control_unit_unique_id, ControlUnitStateVaribleListModel::StateVariableTypeAlarmDEV),
    warning_list_model(control_unit_unique_id, ControlUnitStateVaribleListModel::StateVariableTypeAlarmCU),
    dataset_output_table_model(control_unit_unique_id,
                               chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT),
    dataset_input_table_model(control_unit_unique_id,
                              chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT) {
    ui->setupUi(this);
}

ControlUnitEditor::~ControlUnitEditor() {
    delete ui;
}

void ControlUnitEditor::initUI() {
    setWindowTitle(QString("Control unit '%1' editor").arg(control_unit_unique_id));

    //init tab signal
    connect(ui->tabWidget,
            SIGNAL(currentChanged(int)),
            SLOT(tabIndexChanged(int)));

    ui->pushButtonRecoverError->setVisible(false);
    ui->pushButtonEditInstance->setEnabled(false);
    //add model to table
    ui->tableViewOutputChannel->setModel(&dataset_output_table_model);
    ui->tableViewOutputChannel->setColumnToContents(0, true);
    ui->tableViewOutputChannel->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableViewInputChannels->setModel(&dataset_input_table_model);
    ui->tableViewInputChannels->setColumnToContents(0, true);
    ui->tableViewInputChannels->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //compose logic on switch
    //for load button
    logic_switch_aggregator.addNewLogicSwitch("cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("cu_can_operate", "us_alive","online");
    logic_switch_aggregator.addKeyRefValue("cu_can_operate", "us_state",chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOAD);

    logic_switch_aggregator.addNewLogicSwitch("load");
    logic_switch_aggregator.connectSwitch("load", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("load", "cu_alive","offline");
    logic_switch_aggregator.addKeyRefValue("load", "cu_alive","not_found");
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("load", ui->pushButtonLoadAction, "enabled", true, false);

    logic_switch_aggregator.addNewLogicSwitch("unload");
    logic_switch_aggregator.connectSwitch("unload", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("unload", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("unload", "cu_state", chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOAD);
    logic_switch_aggregator.addKeyRefValue("unload", "cu_state", chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINIT);
    logic_switch_aggregator.addKeyRefValue("unload", "cu_state", chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_FERROR);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("unload", ui->pushButtonUnload, "enabled", true, false);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("unload", ui->pushButtonInitAction, "enabled", true, false);

    logic_switch_aggregator.addNewLogicSwitch("start");
    logic_switch_aggregator.connectSwitch("start", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("start", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("start", "cu_state",chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_INIT);
    logic_switch_aggregator.addKeyRefValue("start", "cu_state",chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("start", ui->pushButtonStartAction, "enabled", true, false);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("start", ui->pushButtonDeinitAction, "enabled", true, false);

    logic_switch_aggregator.addNewLogicSwitch("stop");
    logic_switch_aggregator.connectSwitch("stop", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("stop", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("stop", "cu_state",chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_START);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("stop", ui->pushButtonStopAction, "enabled", true, false);

    logic_switch_aggregator.addNewLogicSwitch("update_property");
    logic_switch_aggregator.connectSwitch("update_property", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("update_property", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("update_property", "cu_state", chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_INIT);
    logic_switch_aggregator.addKeyRefValue("update_property", "cu_state", chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_START);
    logic_switch_aggregator.addKeyRefValue("update_property", "cu_state", chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_STOP);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("update_property", ui->pushButtonSetRunScheduleDelay, "enabled", true, false);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("update_property", ui->lineEditRunScheduleDelay, "enabled", true, false);

    logic_switch_aggregator.addNewLogicSwitch("recover_error");
    logic_switch_aggregator.connectSwitch("recover_error", "cu_can_operate");
    logic_switch_aggregator.addKeyRefValue("recover_error", "cu_alive","online");
    logic_switch_aggregator.addKeyRefValue("recover_error", "cu_state", chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_RERROR);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("recover_error", ui->pushButtonRecoverError, "enabled", true, false);
    logic_switch_aggregator.attachObjectAttributeToSwitch<bool>("recover_error", ui->pushButtonRecoverError, "visible", true, false);

    //set the status on not_found for either the control unit and unit serve
    logic_switch_aggregator.broadcastCurrentValueForKey("cu_alive", getStatusString(0));
    logic_switch_aggregator.broadcastCurrentValueForKey("us_alive", getStatusString(0));

    //connect slot to logic switch signals
    connect(&logic_switch_aggregator,
            SIGNAL(stateChangedOnSwitch(QString,bool)),
            SLOT(onLogicSwitchChangeState(QString,bool)));

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
    //cu health indicator
    ui->ledIndicatorHealtTSControlUnit->setNodeUID(control_unit_unique_id);

    //control unit status
    ui->labelControlUnitState->setHealthAttribute(CNodeHealthLabel::HealthOperationalState);
    ui->labelControlUnitState->setNodeUID(control_unit_unique_id);

    //unit server state
    ui->labelUnitServerState->setHealthAttribute(CNodeHealthLabel::HealthOperationalState);

    //chaos label for the current thread schedule delay
    ui->labelRunScheduleDelaySet->setNodeUID(control_unit_unique_id);
    ui->labelRunScheduleDelaySet->setDatasetAttributeName(chaos::ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY);
    ui->labelRunScheduleDelaySet->setDatasetType(CControlUnitDatasetLabel::DatasetTypeSystem);

    //chaos label for the current output dataset push rate
    ui->chaosLabelDSOutputPushRate->setNodeUID(control_unit_unique_id);
    ui->chaosLabelDSOutputPushRate->setHealthAttribute(CNodeHealthLabel::HealthCustomAttribute);
    ui->chaosLabelDSOutputPushRate->setCustomHealthAttribute(chaos::ControlUnitHealtDefinitionValue::CU_HEALT_OUTPUT_DATASET_PUSH_RATE);

    //thread schedule update
    ui->lineEditRunScheduleDelay->setValidator(new QIntValidator(0,60000000));

    //storage type
    ui->widgetStorageType->setNodeUID(control_unit_unique_id);

    //driver bypass
    ui->pushButtonDriverBypass->setNodeUID(control_unit_unique_id);
    ui->pushButtonDriverBypass->setPropertyGroupName("property_abstract_control_unit");
    ui->pushButtonDriverBypass->setPropertyName(chaos::ControlUnitDatapackSystemKey::BYPASS_STATE);

    //start monitoring
    manageMonitoring(true);

    //enable log widget
    ui->widgetChaosNodeLog->setNodeUID(control_unit_unique_id);
    ui->widgetChaosNodeLog->initChaosContent();

    //enable
    ui->widgetNodeResource->setNodeUID(control_unit_unique_id);
    ui->widgetNodeResource->initChaosContent();

    //manage  alarm and warning list
    ui->splitterOutputDatasets->setStretchFactor(1,0);
    //reset the splitter in mode to hide the lists
    on_checkBoxShowAlarms_clicked();
    ui->listViewAlarm->setModel(&alarm_list_model);
    ui->listViewWarning->setModel(&warning_list_model);
    //manage command stat
    ui->widgetCommandStatistic->setVisible(false);
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
        ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(control_unit_unique_id.toStdString(),
                                                                           node_monitor::ControllerTypeNode,
                                                                           this);
        ui->labelControlUnitState->initChaosContent();
        ui->labelRunScheduleDelaySet->initChaosContent();
        ui->ledIndicatorHealtTSControlUnit->initChaosContent();
        ui->chaosLabelDSOutputPushRate->initChaosContent();
        ui->widgetStorageType->initChaosContent();
        ui->pushButtonDriverBypass->initChaosContent();
    }else{
        if(unit_server_parent_unique_id.size()) {
            //remove old unit server for healt
            ui->labelUnitServerState->deinitChaosContent();
            ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(unit_server_parent_unique_id.toStdString(),
                                                                                  node_monitor::ControllerTypeNode,
                                                                                  this);
            ui->ledIndicatorHealtTSUnitServer->deinitChaosContent();
        }
        ui->widgetStorageType->deinitChaosContent();
        ui->pushButtonDriverBypass->deinitChaosContent();
        ui->labelControlUnitState->deinitChaosContent();
        ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(control_unit_unique_id.toStdString(),
                                                                              node_monitor::ControllerTypeNode,
                                                                              this);
        ui->labelRunScheduleDelaySet->deinitChaosContent();
        ui->ledIndicatorHealtTSControlUnit->deinitChaosContent();
        ui->chaosLabelDSOutputPushRate->deinitChaosContent();
    }
}


bool ControlUnitEditor::isClosing() {
    //disable resource widget
    ui->widgetNodeResource->deinitChaosContent();
    //disable log widget
    ui->widgetChaosNodeLog->deinitChaosContent();

    dataset_input_table_model.setAttributeMonitoring(false);
    dataset_output_table_model.setAttributeMonitoring(false);
    //remove monitoring on cu and us
    manageMonitoring(false);
    alarm_list_model.untrack();
    warning_list_model.untrack();
    if(control_unit_subtype.compare(chaos::NodeType::NODE_SUBTYPE_BATCH_CONTROL_UNIT) == 0){
        ui->widgetCommandStatistic->deinitChaosContent();
    }
    return true;
}

QString ControlUnitEditor::getStatusString(int status) {
    switch(status) {
    case OnlineStateNotFound:
    case OnlineStateUnknown:
        return QString("not_found");
        break;
    case OnlineStateOFF:
        return QString("offline");
        break;
    case OnlineStateON:
        return QString("online");
        break;
    }
}

void ControlUnitEditor::onApiDone(const QString& tag,
                                  QSharedPointer<CDataWrapper> api_result) {
    if(tag.compare(TAG_CU_INFO) == 0) {
        if(api_result.isNull()) return;
        fillInfo(api_result);
    } else if(tag.compare(TAG_CU_DATASET) == 0) {
        submitApiResult(QString(TAG_CU_INSTANCE),
                        GET_CHAOS_API_PTR(control_unit::GetInstance)->execute(control_unit_unique_id.toStdString()));
        if(api_result.isNull()) return;
        fillDataset(api_result);
    } else if(tag.compare(TAG_CU_INSTANCE) == 0) {
        if(api_result.isNull()) return;
        //enable the button for editing instance
        bool has_node_parent = api_result->hasKey(chaos::NodeDefinitionKey::NODE_PARENT);
        ui->pushButtonEditInstance->setEnabled(has_node_parent);
        if(has_node_parent){
            const QString new_u_s = QString::fromStdString(api_result->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT));
            if(unit_server_parent_unique_id.compare(new_u_s) != 0) {
                //whe ahve unit server changed
                if(unit_server_parent_unique_id.size()) {
                    //remove old unit server for healt
                    ui->labelUnitServerState->deinitChaosContent();
                    ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(unit_server_parent_unique_id.toStdString(),
                                                                                          node_monitor::ControllerTypeNode,
                                                                                          this);
                    ui->ledIndicatorHealtTSUnitServer->deinitChaosContent();
                }
                unit_server_parent_unique_id = new_u_s;

                ui->labelUnitServerState->setNodeUID(unit_server_parent_unique_id);
                ui->labelUnitServerState->initChaosContent();
                ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(unit_server_parent_unique_id.toStdString(),
                                                                                   node_monitor::ControllerTypeNode,
                                                                                   this);

                ui->ledIndicatorHealtTSUnitServer->setNodeUID(unit_server_parent_unique_id);
                ui->ledIndicatorHealtTSUnitServer->initChaosContent();
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
    PresenterWidget::onApiDone(tag,
                               api_result);
}

void ControlUnitEditor::fillInfo(const QSharedPointer<chaos::common::data::CDataWrapper>& node_info) {
    //check control unit type
    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_SUB_TYPE)) {
        control_unit_subtype = QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_SUB_TYPE));
        if(control_unit_subtype.compare(chaos::NodeType::NODE_SUBTYPE_BATCH_CONTROL_UNIT) == 0){
            ui->widgetCommandStatistic->setVisible(true);
            ui->widgetCommandStatistic->setNodeUID(control_unit_unique_id);
            ui->widgetCommandStatistic->initChaosContent();
        }
    }
    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
        QString addr_host;
        if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_HOST_NAME)) {
            addr_host = QString("%1[%2]").arg(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR).c_str()).arg(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_HOST_NAME).c_str());
        } else {
            addr_host = QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR));
        }
        ui->labelRemoteAddress->setText(addr_host);
    } else {
        ui->labelRemoteAddress->setText(tr("---"));
    }

    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)) {
        ui->labelRpcDomain->setText(QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)));
    } else {
        ui->labelRpcDomain->setText(tr("---"));
    }

    if(node_info->hasKey(chaos::NodeDefinitionKey::NODE_TYPE)) {
        ui->labelType->setText(QString::fromStdString(node_info->getStringValue(chaos::NodeDefinitionKey::NODE_TYPE)));
    } else {
        ui->labelType->setText(tr("---"));
    }
}

void ControlUnitEditor::templateSaved(const QString& tempalte_name,
                                      const QString& command_uid) {
    //after temaplte has been saved update the search
    updateTemplateSearch();
}

void ControlUnitEditor::onLogicSwitchChangeState(const QString& switch_name,
                                                 bool switch_activate) {
    //    if(switch_name.compare("unload") == 0) {
    //        updateAllControlUnitInfomration();
    //    }
}

void ControlUnitEditor::handleSelectionChangedOnListWiew(const QItemSelection& selection,
                                                         const QItemSelection &previous_selected) {
    QObject* sender = QObject::sender()->parent();
    if(ui->listViewCommandList == sender) {
        ui->pushButtonAddNewCommadInstance->setEnabled(ui->listViewCommandList->selectionModel()->selectedRows().size()==1);
    }else if(ui->listViewCommandInstance == sender){
        unsigned int selected_template_count = ui->listViewCommandInstance->selectionModel()->selectedRows().size();
        ui->pushButtonEditInstance->setEnabled(selected_template_count);
        ui->pushButtonRemoveInstance->setEnabled(selected_template_count);
        ui->pushButtonCreateInstance->setEnabled(selected_template_count);
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
    std::vector< ChaosSharedPtr< control_unit::ControlUnitInputDatasetChangeSet > > value_set_array;
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
    if(item->data.canConvert< QSharedPointer<CDataWrapper> >() ){
        QSharedPointer<CDataWrapper> raw_data = item->data.value< QSharedPointer<CDataWrapper> >();
        ControlUnitCommandTemplateEditor  *template_editor= new ControlUnitCommandTemplateEditor(QString::fromStdString(raw_data->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)));
        //connect tempalte editor
        connect(template_editor,
                SIGNAL(templateSaved(QString,QString)),
                SLOT(templateSaved(QString,QString)));

        launchPresenterWidget(template_editor);
    }
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

        launchPresenterWidget(template_editor);
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
        launchPresenterWidget(new CommandTemplateInstanceEditor(control_unit_unique_id,
                                                                index.data().toString(),
                                                                index.data(Qt::UserRole).toString()));
    }
}

void ControlUnitEditor::on_pushButtonSetRunScheduleDelay_clicked() {
    chaos::metadata_service_client::api_proxy::node::NodePropertyGroupList property_list;
    ChaosSharedPtr<chaos::common::data::CDataWrapperKeyValueSetter> thread_run_schedule(new chaos::common::data::CDataWrapperInt64KeyValueSetter(chaos::ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY,
                                                                                                                                                    ui->lineEditRunScheduleDelay->text().toLongLong()));
    ChaosSharedPtr<chaos::metadata_service_client::api_proxy::node::NodePropertyGroup> cu_property_group(new chaos::metadata_service_client::api_proxy::node::NodePropertyGroup());
    cu_property_group->group_name = "property_abstract_control_unit";
    cu_property_group->group_property_list.push_back(thread_run_schedule);

    property_list.push_back(cu_property_group);

    submitApiResult(TAG_CU_SET_THREAD_SCHEDULE_DELAY,
                    GET_CHAOS_API_PTR(node::UpdateProperty)->execute(control_unit_unique_id.toStdString(),
                                                                     property_list));
}

void ControlUnitEditor::on_pushButtonRecoverError_clicked() {
    //recover error api call
    std::vector<std::string> uids;
    uids.push_back(control_unit_unique_id.toStdString())            ;
    submitApiResult(TAG_CU_RECOVER_ERROR,
                    GET_CHAOS_API_PTR(control_unit::RecoverError)->execute(uids));
}

void ControlUnitEditor::on_pushButtonOpenInstanceEditor_clicked() {
    launchPresenterWidget(new ControUnitInstanceEditor(unit_server_parent_unique_id,
                                                       control_unit_unique_id,
                                                       true));
}

void ControlUnitEditor::tabIndexChanged(int new_index) {
    switch(new_index){
    case 2:
        ui->widgetChaosNodeLog->updateChaosContent();
        break;
    default:
        break;
    }
}

//node monitor inherited method

void ControlUnitEditor::nodeChangedOnlineState(const std::string& node_uid,
                                               OnlineState old_state,
                                               OnlineState new_state) {
    if(node_uid.compare(control_unit_unique_id.toStdString()) == 0) {
        //state changed for control unit
        logic_switch_aggregator.broadcastCurrentValueForKey("cu_alive", getStatusString(new_state));

        updateAllControlUnitInfomration();
    } else if(node_uid.compare(unit_server_parent_unique_id.toStdString()) == 0) {
        //state changed for unit server
        logic_switch_aggregator.broadcastCurrentValueForKey("us_alive", getStatusString(new_state));
    }
}

void ControlUnitEditor::nodeChangedInternalState(const std::string& node_uid,
                                                 const std::string& old_state,
                                                 const std::string& new_state) {
    if(node_uid.compare(control_unit_unique_id.toStdString()) == 0) {
        logic_switch_aggregator.broadcastCurrentValueForKey("cu_state",  QString::fromStdString(new_state));
        if(restarted && (/*new_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOAD) == 0 ||*/
                         new_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_INIT) == 0 ||
                         new_state.compare(chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_START) == 0 )) {
            qDebug() << "Load dataset for Node" << QString::fromStdString(node_uid) << " that has been restarted";
            //update
            updateAllControlUnitInfomration();
            restarted = false;
        }
        //broadcast cu status to switch
    } else if(node_uid.compare(unit_server_parent_unique_id.toStdString()) == 0) {
        //state changed for unit server
        logic_switch_aggregator.broadcastCurrentValueForKey("us_state", QString::fromStdString(new_state));
    }
}

void ControlUnitEditor::nodeHasBeenRestarted(const std::string& node_uid) {
    if(node_uid.compare(control_unit_unique_id.toStdString()) == 0) {
        qDebug() << "Node" << QString::fromStdString(node_uid) << " restarted";
        restarted = true;
        updateAllControlUnitInfomration();
    }
}

void ControlUnitEditor::on_pushButtonShowPlot_clicked() {
    NodeAttributePlotting *plot_viewer = new NodeAttributePlotting(control_unit_unique_id, NULL);
    plot_viewer->show();
}

void ControlUnitEditor::on_checkBoxShowAlarms_clicked() {
    QList<int> splitter_size;
    if(ui->checkBoxShowAlarms->isChecked()) {
        alarm_list_model.track();
        warning_list_model.track();
        splitter_size.push_back(450);
        splitter_size.push_back(150);
    } else {
        alarm_list_model.untrack();
        warning_list_model.untrack();
        splitter_size.push_back(600);
        splitter_size.push_back(0);
    }
    ui->splitterOutputDatasets->setSizes(splitter_size);
}

void ControlUnitEditor::on_pushButtonUpdateControlUnitInfo_clicked() {
    updateAllControlUnitInfomration();
}
