#include "ControlUnitCommandTemplateEditor.h"
#include "ui_ControlUnitCommandTemplateEditor.h"

#include <QToolTip>
#include <QMessageBox>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;

static const QString TAG_CMD_TEMPLATE_SET = QString("cuct_tmplt_set");
static const QString TAG_CMD_COMMAND_GET = QString("cuct_cmd_get");
static const QString TAG_CMD_FETCH_TEMPLATE_AND_COMMAND = QString("cuct_edit_get_tc");

ControlUnitCommandTemplateEditor::ControlUnitCommandTemplateEditor(const QString& _command_uid) :
    PresenterWidget(NULL),
    parameter_table_model(this),
    ui(new Ui::ControlUnitCommandTemplateEditor),
    in_editing(false){
    ui->setupUi(this);
    ui->labelCommandUID->setText(_command_uid);
}

ControlUnitCommandTemplateEditor::ControlUnitCommandTemplateEditor(const QString &_template_name,
                                                                   const QString &_command_uid) :
    PresenterWidget(NULL),
    parameter_table_model(this),
    ui(new Ui::ControlUnitCommandTemplateEditor),
    in_editing(true){
    ui->setupUi(this);
    ui->lineEditTemplateName->setText(_template_name);
    ui->lineEditTemplateName->setReadOnly(true);
    ui->labelCommandUID->setText(_command_uid);
}

ControlUnitCommandTemplateEditor::~ControlUnitCommandTemplateEditor() {
    delete ui;
}

void ControlUnitCommandTemplateEditor::initUI() {
    setTabTitle("Command Tempalte Editor");
    ui->lineEditTemplateName->setValidator(new QRegExpValidator(QRegExp(tr("[a-zA-z0-9_]{8,255}"))));
    ui->lineEditSubmissionPriority->setValidator(new QIntValidator(0, 100, this));
    ui->lineEditSubmissionRunStepDelay->setValidator(new QIntValidator(0, 60000000, this));
    ui->lineEditSubmissionRetry->setValidator(new QIntValidator(0, 1000000, this));

    ui->tableViewParameterList->setModel(&parameter_table_model);
    ui->tableViewParameterList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if(in_editing) {
        //load template and command
        submitApiResult(TAG_CMD_FETCH_TEMPLATE_AND_COMMAND,
                        GET_CHAOS_API_PTR(node::GetCommandAndTemplateDescription)->execute(ui->lineEditTemplateName->text().toStdString(),
                                                                                            ui->labelCommandUID->text().toStdString()));
    } else {
        submitApiResult(TAG_CMD_COMMAND_GET,
                        GET_CHAOS_API_PTR(node::CommandGet)->execute(ui->labelCommandUID->text().toStdString()));
    }
}

bool ControlUnitCommandTemplateEditor::isClosing() {
    return true;
}

void ControlUnitCommandTemplateEditor::onApiDone(const QString& tag,
                                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare(TAG_CMD_TEMPLATE_SET) == 0) {
        //emit signal for saved template
        emit(templateSaved( ui->lineEditTemplateName->text(),
                            ui->labelCommandUID->text()));
        closeTab();
    } else if(tag.compare(TAG_CMD_COMMAND_GET) == 0) {
        //set the command description
        setCommandDescription(api_result);
    } else if(tag.compare(TAG_CMD_FETCH_TEMPLATE_AND_COMMAND) == 0) {
        if(!api_result->hasKey("command_description") ||
                !api_result->isCDataWrapperValue("command_description")) {
            showInformation(tr("Error"), tr("Command Fetch"), QString("The  uid '%1%' for the command has not been found").arg(ui->labelCommandUID->text()));
            return;
        }
        if(!api_result->hasKey("template_description") ||
                !api_result->isCDataWrapperValue("template_description")) {
            showInformation(tr("Error"), tr("Template Fetch"), QString("The  temaplte '%1%' has not been found").arg(ui->lineEditTemplateName->text()));
            return;
        }
        setCommandDescription(QSharedPointer<CDataWrapper>(api_result->getCSDataValue("command_description")));
        setTemplateDescription(QSharedPointer<CDataWrapper>(api_result->getCSDataValue("template_description")));
    }
    PresenterWidget::onApiDone(tag,
                               api_result);
}

void ControlUnitCommandTemplateEditor::setCommandDescription(QSharedPointer<chaos::common::data::CDataWrapper> _command_description) {
    command_description = _command_description;
    if(command_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_ALIAS)){
        ui->labelCommandName->setText(QString::fromStdString(command_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS)));
    }
    if(command_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)){
        ui->labelCommandUID->setText(QString::fromStdString(command_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)));
    }
    //update attribute within the model
    parameter_table_model.updateAttribute(command_description);
}

//used for editing
void ControlUnitCommandTemplateEditor::setTemplateDescription(QSharedPointer<chaos::common::data::CDataWrapper> _template_description) {
    uint32_t temp_int = 0;
    uint64_t temp_long = 0;
    //reappli submission rule
    if(_template_description->hasKey(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL)){
        temp_int = _template_description->getInt32Value(BatchCommandSubmissionKey::COMMAND_EXECUTION_CHANNEL);
        ui->comboBoxSubmissionExecutionChannel->setCurrentIndex(temp_int);
    }
    if(_template_description->hasKey(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32)){
        temp_int = _template_description->getInt32Value(BatchCommandSubmissionKey::SUBMISSION_RULE_UI32);
        ui->comboBoxSubmissionRule->setCurrentIndex(temp_int);
    }
    if(_template_description->hasKey(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32)){
        temp_int = _template_description->getInt32Value(BatchCommandSubmissionKey::SUBMISSION_PRIORITY_UI32);
        ui->lineEditSubmissionPriority->setText(QString::number(temp_int));
    }
    if(_template_description->hasKey(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32)){
        temp_int = _template_description->getInt32Value(BatchCommandSubmissionKey::SUBMISSION_RETRY_DELAY_UI32);
        ui->lineEditSubmissionRetry->setText(QString::number(temp_int));
    }
    if(_template_description->hasKey(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64)){
        temp_long = _template_description->getInt64Value(BatchCommandSubmissionKey::SCHEDULER_STEP_TIME_INTERVALL_UI64);
        ui->lineEditSubmissionRunStepDelay->setText(QString::number(temp_long));
    }

    //reapply attribute value
    parameter_table_model.applyTemplate(_template_description);
}

boost::shared_ptr<node::CommandTemplate> ControlUnitCommandTemplateEditor::getTemplateDescription() {
    boost::shared_ptr<node::CommandTemplate> result(new node::CommandTemplate());
    //set base information

    result->template_name = ui->lineEditTemplateName->text().toStdString();
    if(command_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)){
        result->command_unique_id = command_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
    }

    //fetch all set parameter
    parameter_table_model.fillTemplate(*result);

    //set the rule and scheduler setting
    result->submission_rule = ui->comboBoxSubmissionRule->currentIndex();
    result->submission_priority = ui->lineEditSubmissionPriority->text().toInt();
    result->schedule_step_delay = ui->lineEditSubmissionRunStepDelay->text().toLongLong();
    result->submission_retry_delay = ui->lineEditSubmissionRetry->text().toInt();
    result->execution_channel = ui->comboBoxSubmissionExecutionChannel->currentIndex();
    return result;
}

void ControlUnitCommandTemplateEditor::reset() {
    //reset all set value
    ui->lineEditTemplateName->setText("");
    ui->lineEditSubmissionPriority->setText("");
    ui->lineEditSubmissionRunStepDelay->setText("");
    ui->lineEditSubmissionRetry->setText("");
    ui->comboBoxSubmissionRule->setCurrentIndex(0);
    ui->comboBoxSubmissionExecutionChannel->setCurrentIndex(0);
    parameter_table_model.resetChanges();
}

#define CHEC_TEXT_VALIDATOR(u)\
    if(u->hasAcceptableInput() == false) {\
    QMessageBox::information(this, QString("Validation input value"), QString("The value for %1 is invalid").arg(u->accessibleName()));\
    return;\
    }

void ControlUnitCommandTemplateEditor::on_pushButtonSave_clicked() {
    QString param_in_error;
    CHEC_TEXT_VALIDATOR(ui->lineEditTemplateName)
    CHEC_TEXT_VALIDATOR(ui->lineEditSubmissionPriority)
    CHEC_TEXT_VALIDATOR(ui->lineEditSubmissionRunStepDelay)
    CHEC_TEXT_VALIDATOR(ui->lineEditSubmissionRetry)
            //check the validation on param rule
    if(!parameter_table_model.validation(param_in_error)){
        QMessageBox::information(this, tr("validation Error"), QString("The attribute '%1' cannot be validated (it can be mandatory and not valorized or parametrized)").arg(param_in_error));
        return;
    }

    std::vector< boost::shared_ptr<node::CommandTemplate> >template_list;
    template_list.push_back(getTemplateDescription());
    submitApiResult(TAG_CMD_TEMPLATE_SET,
                    GET_CHAOS_API_PTR(node::CommandTemplateSet)->execute(template_list));
}

void ControlUnitCommandTemplateEditor::on_pushButtonCancel_clicked() {
    closeTab();
}

void ControlUnitCommandTemplateEditor::on_pushButtonReset_clicked() {
    reset();
}
