#include "CommandTemplateInstanceEditor.h"
#include "ui_CommandTemplateInstanceEditor.h"
#include "../../widget/CDSAttrQLineEdit.h"

#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>

const QString TAG_CMD_FETCH_TEMPLATE_AND_COMMAND = QString("cmd_fetch_template_command");
const QString TAG_CMD_INSTANCE_SUBMIT = QString("cmd_instance_sumission");

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;

CommandTemplateInstanceEditor::CommandTemplateInstanceEditor(const QString& _node_uid,
                                                             const QString& _template_name,
                                                             const QString& _command_uid) :
    PresenterWidget(NULL),
    ui(new Ui::CommandTemplateInstanceEditor),
    node_uid(_node_uid),
    template_name(_template_name),
    command_uid(_command_uid),
    close_after_submition(false){
    ui->setupUi(this);
}

CommandTemplateInstanceEditor::~CommandTemplateInstanceEditor() {
    delete ui;
}

void CommandTemplateInstanceEditor::keyPressEvent(QKeyEvent *key_evt) {
    switch(key_evt->key()) {
    case Qt::Key_Control:
        close_after_submition = true;
        ui->pushButtonSubmitInstance->setText("Submit Instance and Close");
        break;

    default:
        break;
    }
}

void CommandTemplateInstanceEditor::keyReleaseEvent(QKeyEvent *key_evt) {
    switch(key_evt->key()) {
    case Qt::Key_Control:
        close_after_submition = false;
        ui->pushButtonSubmitInstance->setText("Submit Instance");
        break;

    default:
        break;
    }
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

bool CommandTemplateInstanceEditor::isClosing() {
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
    } else if(tag.compare(TAG_CMD_INSTANCE_SUBMIT) == 0) {
        //instance has bee saved so we can close the panel
        closeTab();
    }
}

boost::shared_ptr<node::TemplateSubmission> CommandTemplateInstanceEditor::getTempalteSubmissionTask() {
    bool ok = false;
    boost::shared_ptr<node::TemplateSubmission> submission_info(new node::TemplateSubmission());
    submission_info->node_unique_id = node_uid.toStdString();
    submission_info->template_name = template_name.toStdString();
    submission_info->command_unique_id = command_uid.toStdString();

    QMapIterator<QString, CDSAttrQLineEdit*> iter(map_attr_name_value_editor);
    while(iter.hasNext()){
        iter.next();
        if(!iter.value()->chaosAttributeValueSetter()->isValid()) {
            showInformation(tr("Submission"), tr("validation"), QString("The attribute %1% is not valid").arg(iter.key()));
            return boost::shared_ptr<node::TemplateSubmission>();
        }
        boost::shared_ptr<CDataWrapperKeyValueSetter> setter = iter.value()->chaosAttributeValueSetter()->getCDataWrapperValueSetter(&ok);
        if(!ok) {
            showInformation(tr("Submission"), tr("validation"), QString("The attribute %1% has an invalid value").arg(iter.key()));
            return boost::shared_ptr<node::TemplateSubmission>();
        }
        submission_info->parametrized_attribute_value.push_back(setter);
    }
    return submission_info;
}

void CommandTemplateInstanceEditor::submitInstance() {
    node::TemplateSubmissionList submission_list;
    boost::shared_ptr<node::TemplateSubmission>  instance = getTempalteSubmissionTask();
    if(instance.get() == NULL) return;
    submission_list.push_back(instance);
    //whe have all submission pack completed

    submitApiResult(TAG_CMD_INSTANCE_SUBMIT,
                    GET_CHAOS_API_PTR(node::CommandTemplateSubmit)->execute(submission_list));
}

void CommandTemplateInstanceEditor::configureForTemplate(QSharedPointer<CDataWrapper> template_description,
                                                         QSharedPointer<CDataWrapper> command_description) {
    //set the form for insert the requested value by the template
    int heigh_to_add = 0;
    QSharedPointer<CommandReader> command_reader(new CommandReader(command_description));
    QList< QSharedPointer<CommandParameterReader> > parameter_reader_list = command_reader->getCommandParameterList();
    foreach (QSharedPointer<CommandParameterReader> param_reader,
             parameter_reader_list) {
        qDebug() << "check parameter: " << param_reader->getName() ;

        if(template_description->isNullValue(param_reader->getName().toStdString())){
            //need to be requested to the user
            QLabel *description = new QLabel(param_reader->getName(), this);
            if(param_reader->isMandatory()) {
                description->setStyleSheet("QLabel { color : #FA8989; }");
            }
            CDSAttrQLineEdit *editor = new CDSAttrQLineEdit(param_reader, this);
            ui->formLayoutInputParameter->addRow(description,
                                                 editor);
            editor->setStyleSheet("CDSAttrQLineEdit {font-size: 11pt;}");
            editor->setMaximumHeight(20);
            heigh_to_add += 24;
            map_attr_name_value_editor.insert(param_reader->getName(), editor);
        }
    }
    setMinimumHeight(minimumHeight()+heigh_to_add);
    parentWidget()->setGeometry(parentWidget()->geometry().adjusted(0,0,0,heigh_to_add));
}
