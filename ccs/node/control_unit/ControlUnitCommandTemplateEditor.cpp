#include "ControlUnitCommandTemplateEditor.h"
#include "ui_ControlUnitCommandTemplateEditor.h"

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
    ui->lineEditTemplateName->setValidator(new QRegExpValidator(QRegExp(tr("[a-zA-z0-9_]*"))));
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
    //update attribute within the model
    parameter_table_model.updateAttribute(command_description);
}

void ControlUnitCommandTemplateEditor::on_buttonBox_accepted() {

}

QSharedPointer<chaos::common::data::CDataWrapper> ControlUnitCommandTemplateEditor::getTempalteDescription() {
    QSharedPointer<control_unit::CommandTemplate> result(new control_unit::CommandTemplate());
    result->template_name = ui->lineEditTemplateName->text().toStdString();
    if(command_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_ALIAS)) {
        result->command_alias = command_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS);
    }
    parameter_table_model.fillTemplate(*result);
}

void ControlUnitCommandTemplateEditor::on_buttonBox_clicked(QAbstractButton *button) {
   if(ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole){
       //reset all set value
       parameter_table_model.resetChanges();
   }
}
