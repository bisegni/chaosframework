#include "ControlUnitCommandTemplateEditor.h"
#include "ui_ControlUnitCommandTemplateEditor.h"

using namespace chaos::common::batch_command;

ControlUnitCommandTemplateEditor::ControlUnitCommandTemplateEditor(QWidget *parent) :
    QDialog(parent),
    parameter_table_model(this),
    ui(new Ui::ControlUnitCommandTemplateEditor) {
    ui->setupUi(this);
    setModal(true);
    setWindowTitle(tr("Command Template Editor"));
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
    if(_command_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_ALIAS)){
        ui->labelCommandName->setText(QString::fromStdString(_command_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS)));
    }
    //update attribute within the model
    parameter_table_model.updateAttribute(_command_description);
}

void ControlUnitCommandTemplateEditor::on_buttonBox_accepted() {

}
