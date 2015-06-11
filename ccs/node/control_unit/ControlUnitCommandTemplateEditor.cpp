#include "ControlUnitCommandTemplateEditor.h"
#include "ui_ControlUnitCommandTemplateEditor.h"

#include <QToolTip>
#include <QMessageBox>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;

ControlUnitCommandTemplateEditor::ControlUnitCommandTemplateEditor(QWidget *parent) :
    QDialog(parent),
    parameter_table_model(this),
    ui(new Ui::ControlUnitCommandTemplateEditor) {
    ui->setupUi(this);
    setModal(true);
    setWindowTitle(tr("Command Template Editor"));
    ui->lineEditTemplateName->setValidator(new QRegExpValidator(QRegExp(tr("[a-zA-z0-9_]{8,255}"))));
    ui->lineEditSubmissionPriority->setValidator(new QIntValidator(0, 100, this));
    ui->lineEditSubmissionRunStepDelay->setValidator(new QIntValidator(0, 60000000, this));
    ui->lineEditSubmissionRetry->setValidator(new QIntValidator(0, 1000000, this));

    ui->tableViewParameterList->setModel(&parameter_table_model);
    ui->tableViewParameterList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

ControlUnitCommandTemplateEditor::~ControlUnitCommandTemplateEditor() {
    delete ui;
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
    result->submission_rule = ui->comboBoxSubmissionRule->currentData().toInt();
    result->submission_priority = ui->lineEditSubmissionPriority->text().toInt();
    result->schedule_step_delay = ui->lineEditSubmissionRunStepDelay->text().toLongLong();
    result->submission_retry_delay = ui->lineEditSubmissionRetry->text().toInt();
    result->execution_channel = ui->comboBoxSubmissionExecutionChannel->currentData().toInt();
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

void ControlUnitCommandTemplateEditor::on_pushButtonReset_clicked() {
    reset();
}


#define CHEC_TEXT_VALIDATOR(u)\
    if(u->hasAcceptableInput() == false) {\
    QMessageBox::information(this, QString("Validation input value"), QString("The value for %1 is invalid").arg(u->accessibleName()));\
    return;\
    }

void ControlUnitCommandTemplateEditor::on_pushButtonSave_clicked() {
    CHEC_TEXT_VALIDATOR(ui->lineEditTemplateName)
    CHEC_TEXT_VALIDATOR(ui->lineEditSubmissionPriority)
    CHEC_TEXT_VALIDATOR(ui->lineEditSubmissionRunStepDelay)
    CHEC_TEXT_VALIDATOR(ui->lineEditSubmissionRetry)
    //emit the singal for complete editing
    emit saveTemplate(getTemplateDescription());
}


void ControlUnitCommandTemplateEditor::on_pushButtonCancel_clicked() {
    reject();
}
