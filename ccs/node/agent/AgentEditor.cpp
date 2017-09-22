#include "AgentEditor.h"
#include "AgentNodeAssociationEditor.h"
#include "../unit_server/UnitServerEditor.h"
#include "../../data/delegate/TwoLineInformationListItemDelegate.h"
#include "ui_AgentEditor.h"

#include <QFontDatabase>

#define START_NODE      "Launch Node"
#define STOP_NODE       "Stop Node"
#define KILL_NODE       "Kill Node"
#define RESTART_NODE    "Restart Node"
#define START_NODE_LOG  "Start Node Logging"
#define STOP_NODE_LOG   "Stop Node Logging"
#define EDIT_NODE       "Edit Node"

using namespace chaos::metadata_service_client;
using namespace chaos::service_common::data::agent;
using namespace chaos::metadata_service_client::api_proxy;

AgentEditor::AgentEditor(const QString& _agent_uid,
                         QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::AgentEditor),
    agent_uid(_agent_uid){
    ui->setupUi(this);
    nodeAssociatedListModel.setAgent(agent_uid);
    setWindowTitle(QString("Agen %1 editor").arg(agent_uid));

}

AgentEditor::~AgentEditor() {
    delete ui;
}

void AgentEditor::initUI() {
    ui->pushButtonEditAssociatedNode->setEnabled(false);
    ui->pushButtonRemoveAssociatedNode->setEnabled(false);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->listViewNodeAssociated->setFont(fixedFont);
    ui->listViewNodeAssociated->setModel(&nodeAssociatedListModel);
    ui->listViewNodeAssociated->setAcceptDrops(true);
    ui->listViewNodeAssociated->setDropIndicatorShown(true);
    ui->listViewNodeAssociated->setItemDelegate(new TwoLineInformationListItemDelegate(ui->listViewNodeAssociated));
    connect(ui->listViewNodeAssociated->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listViewNodeAssociated,
            SIGNAL(doubleClicked(QModelIndex)),
            SLOT(doubleClicked(QModelIndex)));

    QVector< QPair<QString, QVariant> > cm;
    cm.push_back(QPair<QString, QVariant>(START_NODE, QVariant()));
    cm.push_back(QPair<QString, QVariant>(STOP_NODE, QVariant()));
    cm.push_back(QPair<QString, QVariant>(KILL_NODE, QVariant()));
    cm.push_back(QPair<QString, QVariant>(RESTART_NODE, QVariant()));
    cm.push_back(QPair<QString, QVariant>(START_NODE_LOG, QVariant()));
    cm.push_back(QPair<QString, QVariant>(STOP_NODE_LOG, QVariant()));
    cm.push_back(QPair<QString, QVariant>(EDIT_NODE, QVariant()));
    registerWidgetForContextualMenu(ui->listViewNodeAssociated,
                                    cm,
                                    false);
    on_pushButtonUpdateList_clicked();

    ui->labelAgentHealthStatus->setNodeUID(agent_uid);
    ui->ledIndicatorAgentStatus->setNodeUID(agent_uid);
    ui->labelAgentHealthStatus->setHealthAttribute(CNodeHealthLabel::HealthOperationalState);
    ui->labelAgentHealthStatus->initChaosContent();
    ui->ledIndicatorAgentStatus->initChaosContent();

    //register for log
    ChaosMetadataServiceClient::getInstance()->registerEventHandler(this);
}

bool AgentEditor::isClosing() {
    ChaosMetadataServiceClient::getInstance()->deregisterEventHandler(this);
    ui->ledIndicatorAgentStatus->deinitChaosContent();
    ui->labelAgentHealthStatus->deinitChaosContent();
    return true;
}

void AgentEditor::updateUI() {
    ui->labelWorkingDirectory->setText(QString::fromStdString(agent_insance.working_directory));
    ui->labelWorkingDirectory->setToolTip(QString::fromStdString(agent_insance.working_directory));
    ui->labelRPCInfo->setText(QString("%1[%2]").arg(agent_insance.rpc_addr.c_str()).arg(agent_insance.hostname.c_str()));
}

void AgentEditor::on_pushButtonUpdateList_clicked() {
    submitApiResult("AgentEditor::update",
                    GET_CHAOS_API_PTR(agent::LoadAgentDescription)->execute(agent_uid.toStdString()));
}

void AgentEditor::onApiDone(const QString& tag,
                            QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare("AgentEditor::update") == 0) {
        GET_CHAOS_API_PTR(agent::LoadAgentDescription)->deserialize(api_result.data(),
                                                                    agent_insance);
        nodeAssociatedListModel.updateList();
        QMetaObject::invokeMethod(this,
                                  "updateUI",
                                  Qt::QueuedConnection);
    }
}

void AgentEditor::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    ui->pushButtonRemoveAssociatedNode->setEnabled(selected.size()>0);
    ui->pushButtonEditAssociatedNode->setEnabled(selected.size()>0);
}

void AgentEditor::doubleClicked(const QModelIndex& clickedIndex) {
    if(clickedIndex.isValid() == false) return;
    launchPresenterWidget(new AgentNodeAssociationEditor(agent_uid,
                                                         clickedIndex.data(Qt::UserRole).toString()));
}

void AgentEditor::on_pushButtonEditAssociatedNode_clicked() {
    foreach(QModelIndex index, ui->listViewNodeAssociated->selectionModel()->selectedIndexes()) {
        launchPresenterWidget(new AgentNodeAssociationEditor(agent_uid,
                                                             index.data(Qt::UserRole).toString()));
    }
}

void AgentEditor::on_pushButtonRemoveAssociatedNode_clicked() {
    QModelIndexList selected_index = ui->listViewNodeAssociated->selectionModel()->selectedIndexes();
    ChaosStringVector associated_node;
    foreach(QModelIndex index, selected_index){
        associated_node.push_back(index.data(Qt::UserRole).toString().toStdString());
    }
    submitApiResult("AgentEditor::remove",
                    GET_CHAOS_API_PTR(agent::RemoveNodeAssociation)->execute(agent_uid.toStdString(),
                                                                             associated_node));
}

void AgentEditor::contextualMenuActionTrigger(const QString& cm_title,
                                              const QVariant& cm_data){
    Q_UNUSED(cm_data);
    QModelIndexList index_list = ui->listViewNodeAssociated->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, index_list) {
        if(cm_title.compare(START_NODE) == 0) {
            submitApiResult("AgentEditor::launch_node",
                            GET_CHAOS_API_PTR(agent::NodeOperation)->execute(index.data(Qt::UserRole).toString().toStdString(), NodeAssociationOperationLaunch));
        } else if(cm_title.compare(STOP_NODE) == 0) {
            submitApiResult("AgentEditor::launch_node",
                            GET_CHAOS_API_PTR(agent::NodeOperation)->execute(index.data(Qt::UserRole).toString().toStdString(), NodeAssociationOperationStop));
        } else if(cm_title.compare(KILL_NODE) == 0) {
            submitApiResult("AgentEditor::launch_node",
                            GET_CHAOS_API_PTR(agent::NodeOperation)->execute(index.data(Qt::UserRole).toString().toStdString(), NodeAssociationOperationKill));
        } else if(cm_title.compare(RESTART_NODE) == 0) {
            submitApiResult("AgentEditor::launch_node",
                            GET_CHAOS_API_PTR(agent::NodeOperation)->execute(index.data(Qt::UserRole).toString().toStdString(), NodeAssociationOperationRestart));
        } else if(cm_title.compare(START_NODE_LOG) == 0) {
            submitApiResult("AgentEditor::launch_node",
                            GET_CHAOS_API_PTR(agent::logging::ManageNodeLogging)->execute(index.data(Qt::UserRole).toString().toStdString(), NodeAssociationLoggingOperationEnable));
        } else if(cm_title.compare(STOP_NODE_LOG) == 0) {
            submitApiResult("AgentEditor::launch_node",
                            GET_CHAOS_API_PTR(agent::logging::ManageNodeLogging)->execute(index.data(Qt::UserRole).toString().toStdString(), NodeAssociationLoggingOperationDisable));
        } else if(cm_title.compare(EDIT_NODE) == 0) {
            launchPresenterWidget(new UnitServerEditor(index.data(Qt::UserRole).toString()));
        }
    }
}

void AgentEditor::on_pushButtonStartCheckProcess_clicked() {
    submitApiResult("AgentEditor::start_process_scan",
                    GET_CHAOS_API_PTR(agent::CheckAgentHostedProcess)->execute(agent_uid.toStdString()));
}

void AgentEditor::handleAgentEvent(const std::string& _agent_uid,
                                   const int32_t& _check_result) {
    //if widget i snot visible we do nothing
    if(isVisible() == false) return;

    if(_agent_uid.compare(agent_uid.toStdString()) == 0) {
        on_pushButtonUpdateList_clicked();
    }
}
