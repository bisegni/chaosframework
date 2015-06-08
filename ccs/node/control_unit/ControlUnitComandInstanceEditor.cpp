#include "ControlUnitComandInstanceEditor.h"
#include "ui_ControlUnitComandInstanceEditor.h"

ControlUnitComandInstanceEditor::ControlUnitComandInstanceEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlUnitComandInstanceEditor)
{
    ui->setupUi(this);
    ui->lineEditSubmissionPriority->setValidator(new QIntValidator(0, 100, this));
    ui->lineEditSubmissionRunStepDelay->setValidator(new QIntValidator(0, 60000000, this));
    ui->lineEditSubmissionRetry->setValidator(new QIntValidator(0, 1000000, this));
}

ControlUnitComandInstanceEditor::~ControlUnitComandInstanceEditor()
{
    delete ui;
}
