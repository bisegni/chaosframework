#include "AgentNodeAssociationEditor.h"
#include "ui_AgentNodeAssociationEditor.h"

using namespace chaos::metadata_service_client::api_proxy;

AgentNodeAssociationEditor::AgentNodeAssociationEditor(const QString &_agent_uid,
                                                       const QString &_associated_node_uid,
                                                       QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::AgentNodeAssociationEditor),
    agent_uid(_agent_uid){
    ui->setupUi(this);
    association.associated_node_uid = _associated_node_uid.toStdString();
}

void AgentNodeAssociationEditor::initUI() {
    setWindowTitle(QString("%1 Association Editor").arg(QString::fromStdString(association.associated_node_uid)));
    ui->textEditConfigurationFileContent->setFocus();
    ui->labelAgentUID->setText(agent_uid);
    submitApiResult("AgentNodeAssociationEditor::loadAssociation",
                    GET_CHAOS_API_PTR(agent::LoadNodeAssociation)->execute(agent_uid.toStdString(),
                                                                           association.associated_node_uid));
}

bool AgentNodeAssociationEditor::isClosing() {

}

void AgentNodeAssociationEditor::onApiDone(const QString& tag,
                                           QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare("AgentNodeAssociationEditor::loadAssociation") == 0) {
        //load association completition

        GET_CHAOS_API_PTR(agent::LoadNodeAssociation)->deserialize(api_result.data(),
                                                                   association);
        //fill ui
        QMetaObject::invokeMethod(this,
                                  "updateUI",
                                  Qt::QueuedConnection);
    }
}

AgentNodeAssociationEditor::~AgentNodeAssociationEditor() {
    delete ui;
}

void AgentNodeAssociationEditor::on_pushButtonSaveAssociation_clicked() {
    association.auto_start = ui->checkBoxAutoSave->isChecked();
    association.launch_cmd_line = ui->lineEditCMDLine->text().toStdString();
    association.configuration_file_content = ui->textEditConfigurationFileContent->toPlainText().toStdString();
    submitApiResult("AgentNodeAssociationEditor::saveAssociation",
                    GET_CHAOS_API_PTR(agent::SaveNodeAssociation)->execute(agent_uid.toStdString(),
                                                                           association));
}

void AgentNodeAssociationEditor::updateUI() {
    ui->checkBoxAutoSave->setChecked(association.auto_start);
    ui->lineEditCMDLine->setText(QString::fromStdString(association.launch_cmd_line));
    ui->textEditConfigurationFileContent->setText(QString::fromStdString(association.configuration_file_content));
}

void AgentNodeAssociationEditor::on_pushButtonUpdateAssociationInformation_clicked() {
    submitApiResult("AgentNodeAssociationEditor::loadAssociation",
                    GET_CHAOS_API_PTR(agent::LoadNodeAssociation)->execute(agent_uid.toStdString(),
                                                                           association.associated_node_uid));
}

void AgentNodeAssociationEditor::on_pushButtonLaunchNodeOnAgent_clicked() {
    submitApiResult("AgentNodeAssociationEditor::launchNode",
                    GET_CHAOS_API_PTR(agent::LaunchNode)->execute(association.associated_node_uid));
}
