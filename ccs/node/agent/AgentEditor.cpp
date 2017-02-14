#include "AgentEditor.h"
#include "AgentNodeAssociationEditor.h"

#include "ui_AgentEditor.h"

#define START_NODE "Launch Node"
#define STOP_NODE "Stop Node"
#define RESTART_NODE "Restart Node"

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
    ui->listViewNodeAssociated->setModel(&nodeAssociatedListModel);
    ui->listViewNodeAssociated->setAcceptDrops(true);
    ui->listViewNodeAssociated->setDropIndicatorShown(true);
    connect(ui->listViewNodeAssociated->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listViewNodeAssociated,
            SIGNAL(doubleClicked(QModelIndex)),
            SLOT(doubleClicked(QModelIndex)));

    QVector< QPair<QString, QVariant> > cm;
    cm.push_back(QPair<QString, QVariant>(START_NODE, QVariant()));
    cm.push_back(QPair<QString, QVariant>(STOP_NODE, QVariant()));
    cm.push_back(QPair<QString, QVariant>(RESTART_NODE, QVariant()));
    registerWidgetForContextualMenu(ui->listViewNodeAssociated,
                                    cm,
                                    false);
    on_pushButtonUpdateList_clicked();
}

bool AgentEditor::isClosing() {
    return true;
}

void AgentEditor::updateUI() {
    ui->labelWorkingDirectory->setText(QString::fromStdString(agent_insance.working_directory));
    ui->labelWorkingDirectory->setToolTip(QString::fromStdString(agent_insance.working_directory));
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
                                                         clickedIndex.data().toString()));
}

void AgentEditor::on_pushButtonEditAssociatedNode_clicked() {
    foreach(QModelIndex index, ui->listViewNodeAssociated->selectionModel()->selectedIndexes()) {
        launchPresenterWidget(new AgentNodeAssociationEditor(agent_uid,
                                                             index.data().toString()));
    }
}

void AgentEditor::on_pushButtonRemoveAssociatedNode_clicked() {
    QModelIndexList selected_index = ui->listViewNodeAssociated->selectionModel()->selectedIndexes();
    ChaosStringVector associated_node;
    foreach(QModelIndex index, selected_index){
        associated_node.push_back(index.data().toString().toStdString());
    }
    submitApiResult("AgentEditor::remove",
                    GET_CHAOS_API_PTR(agent::RemoveNodeAssociation)->execute(agent_uid.toStdString(),
                                                                             associated_node));
}

void AgentEditor::contextualMenuActionTrigger(const QString& cm_title,
                                              const QVariant& cm_data){
    Q_UNUSED(cm_data);
    if(cm_title.compare(START_NODE) == 0) {
        QModelIndexList index_list = ui->listViewNodeAssociated->selectionModel()->selectedIndexes();
        foreach(QModelIndex index, index_list) {
            submitApiResult("AgentEditor::launch_node",
                            GET_CHAOS_API_PTR(agent::LaunchNode)->execute(index.data().toString().toStdString()));
        }
    }
}
