#include "AgentEditor.h"
#include "AgentNodeAssociationEditor.h"

#include "ui_AgentEditor.h"

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
    cm.push_back(QPair<QString, QVariant>("Start Node", QVariant()));
    cm.push_back(QPair<QString, QVariant>("Stop Node", QVariant()));
    cm.push_back(QPair<QString, QVariant>("Restart Node", QVariant()));
    registerWidgetForContextualMenu(ui->listViewNodeAssociated,
                                    cm,
                                    false);
    submitApiResult("AgentEditor::update",
                    GET_CHAOS_API_PTR(agent::LoadAgentDescription)->execute(agent_uid.toStdString()));
}

bool AgentEditor::isClosing() {
    return true;
}

void AgentEditor::on_pushButtonUpdateList_clicked() {
    nodeAssociatedListModel.updateList();
}

void AgentEditor::onApiDone(const QString& tag,
                            QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare("AgentEditor::update") == 0) {
        GET_CHAOS_API_PTR(agent::LoadAgentDescription)->deserialize(api_result.data(),
                                                                    agent_insance);
    } else {
        nodeAssociatedListModel.updateList();
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
    qDebug() << "CM:" << cm_title;
}
