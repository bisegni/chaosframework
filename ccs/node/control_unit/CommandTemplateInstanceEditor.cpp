#include "CommandTemplateInstanceEditor.h"
#include "ui_CommandTemplateInstanceEditor.h"
#include "../../widget/CDSAttrQLineEdit.h"

#include <QLineEdit>
#include <QDebug>

const QString TAG_CMD_FETCH_TEMPLATE_AND_COMMAND = QString("cmd_fetch_template_command");

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
    setTabTitle("Instance creation");
    ui->labelNodeUniqueID->setText(node_uid);
    ui->labelTemplateName->setText(template_name);
    //load template
    submitApiResult(TAG_CMD_FETCH_TEMPLATE_AND_COMMAND,
                    GET_CHAOS_API_PTR(node::GetCommandAndTemplateDescription)->execute(template_name.toStdString(),
                                                                                       command_uid.toStdString()));
}

bool CommandTemplateInstanceEditor::canClose() {
    return true;
}

void CommandTemplateInstanceEditor::onApiDone(const QString& tag,
                                              QSharedPointer<CDataWrapper> api_result) {
    if(tag.compare(TAG_CMD_FETCH_TEMPLATE_AND_COMMAND) == 0) {
        if(!api_result->hasKey("command_description") ||
                !api_result->isCDataWrapperValue("command_description")) {
            showInformation(tr("Error"), tr("Command Fetch"), QString("The  uid '%1%' for the command has not been found").arg(command_uid));
            return;
        }
        if(!api_result->hasKey("template_description") ||
                !api_result->isCDataWrapperValue("template_description")) {
            showInformation(tr("Error"), tr("Template Fetch"), QString("The  temaplte '%1%' has not been found").arg(template_name));
            return;
        }

        configureForTemplate(QSharedPointer<CDataWrapper>(api_result->getCSDataValue("template_description")),
                                QSharedPointer<CDataWrapper>(api_result->getCSDataValue("command_description")));
    }
}

void CommandTemplateInstanceEditor::submitInstance() {

}

void CommandTemplateInstanceEditor::configureForTemplate(QSharedPointer<CDataWrapper> template_description,
                                                         QSharedPointer<CDataWrapper>  command_description) {
    //set the form for insert the requested value by the template
    qDebug() << "Tempalte: " << QString::fromStdString(template_description->getJSONString());
    qDebug() << "Command: " << QString::fromStdString(command_description->getJSONString());

    QSharedPointer<CommandReader> command_reader(new CommandReader(command_description));
    QList< QSharedPointer<CommandParameterReader> > parameter_reader_list = command_reader->getCommandParameterList();
    foreach (QSharedPointer<CommandParameterReader> param_reader,
             parameter_reader_list) {
        qDebug() << "check parameter: " << param_reader->getName() ;

        if(template_description->isNullValue(param_reader->getName().toStdString())){
            //need to be requested to the user

            ui->formLayoutInputParameter->addRow(new QLabel(param_reader->getName(), this),
                                                 new CDSAttrQLineEdit(param_reader, this));
        }
    }
    updateGeometry();
    setGeometry(geometry().adjusted(0,0,0,parameter_reader_list.size()*2));
}
