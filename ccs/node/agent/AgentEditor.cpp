#include "AgentEditor.h"
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
    connect(ui->listViewNodeAssociated->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));
    ui->listViewNodeAssociated->setModel(&nodeAssociatedListModel);
    ui->listViewNodeAssociated->setAcceptDrops(true);
    ui->listViewNodeAssociated->setDropIndicatorShown(true);
}

bool AgentEditor::isClosing() {
    return true;
}

void AgentEditor::on_pushButtonAddUnitServer_clicked() {
    QModelIndexList selected_index = ui->listViewNodeAssociated->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, selected_index){
        QString associated_node = index.data().toString();
        submitApiResult("AgentEditor::remove",
                        GET_CHAOS_API_PTR(agent::RemoveNodeAssociation)->execute(agent_uid.toStdString(),
                                                                                 associated_node.toStdString()));
    }
}

void AgentEditor::on_pushButtonRemoveUnitServer_clicked() {

}

void AgentEditor::on_pushButtonUpdateList_clicked() {
    nodeAssociatedListModel.updateList();
}

void AgentEditor::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    ui->pushButtonRemoveUnitServer->setEnabled(selected.size()>0);
}

void AgentEditor::onApiDone(const QString& tag,
                            QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    nodeAssociatedListModel.updateList();
}
