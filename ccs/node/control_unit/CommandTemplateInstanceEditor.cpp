#include "CommandTemplateInstanceEditor.h"
#include "ui_CommandTemplateInstanceEditor.h"

const QString TAG_CMD_FETCH_TEMPLATE = QString("cmd_fetch_template");

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;

CommandTemplateInstanceEditor::CommandTemplateInstanceEditor(const QString& _node_uid,
                                                             const QString& _template_name,
                                                             const QString& _command_uid) :
    PresenterWidget(NULL),
    ui(new Ui::CommandTemplateInstanceEditor),
    node_uid(_node_uid),
    template_name(_template_name),
    command_uid(_command_uid) {
    ui->setupUi(this);
}

CommandTemplateInstanceEditor::~CommandTemplateInstanceEditor() {
    delete ui;
}

void CommandTemplateInstanceEditor::initUI() {
    ui->labelTemplateName->setText(template_name);
    //load template
    submitApiResult(TAG_CMD_TEMPLATE_GET,
                    GET_CHAOS_API_PTR(node::CommandTemplateGet)->execute(ui->lineEditTemplateName->text().toStdString(),
                                                                         ui->labelCommandUID->text().toStdString()));
}

bool CommandTemplateInstanceEditor::canClose() {

}

void CommandTemplateInstanceEditor::onApiDone(const QString& tag,
                                              QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare(TAG_CMD_FETCH_TEMPLATE) == 0) {
        configureForTemplate(api_result);
    }
}

void CommandTemplateInstanceEditor::on_pushButtonSubmitInstance_clicked() {

}

void CommandTemplateInstanceEditor::on_pushButtonClose_clicked() {

}

void CommandTemplateInstanceEditor::configureForTemplate(QSharedPointer<chaos::common::data::CDataWrapper> template_description) {

}
