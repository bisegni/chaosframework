#include "EditableScriptVariableTableModelEditDialog.h"
#include "ui_EditableScriptVariableTableModelEditDialog.h"

using namespace chaos::service_common::data::dataset;

EditableScriptVariableTableModelEditDialog::EditableScriptVariableTableModelEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditableScriptVariableTableModelEditDialog) {
    ui->setupUi(this);
}

EditableScriptVariableTableModelEditDialog::EditableScriptVariableTableModelEditDialog(const AlgorithmVariable &variable,
                                                                                       QWidget *parent):
    QDialog(parent),
    ui(new Ui::EditableScriptVariableTableModelEditDialog),
    editable_variable(variable){
    ui->setupUi(this);
}

EditableScriptVariableTableModelEditDialog::~EditableScriptVariableTableModelEditDialog() {
    delete ui;
}

void EditableScriptVariableTableModelEditDialog::setVariable(const AlgorithmVariable& variable) {
    editable_variable = variable;
    ui->widgetVariableSubtypesEditor->setSubtypeListPtr(&editable_variable.subtype_list);
    fillUIFromVariable();
}

void EditableScriptVariableTableModelEditDialog::fillUIFromVariable() {
    ui->lineEditName->setText(QString::fromStdString(editable_variable.name));
    ui->plainTextEditDescription->setPlainText(QString::fromStdString(editable_variable.description));
    ui->comboBoxTypes->setCurrentIndex(editable_variable.type);
    ui->comboBoxDirection->setCurrentIndex(editable_variable.direction);
    ui->checkBoxMandatory->setChecked(editable_variable.mandatory);
}

void EditableScriptVariableTableModelEditDialog::fillVariableFromUI() {
    editable_variable.name = ui->lineEditName->text().toStdString();
    editable_variable.description = ui->plainTextEditDescription->document()->toPlainText().toStdString();
    editable_variable.type = (chaos::DataType::DataType)ui->comboBoxTypes->currentIndex();
    editable_variable.direction = (chaos::DataType::DataSetAttributeIOAttribute)ui->comboBoxDirection->currentIndex();
    editable_variable.mandatory = ui->checkBoxMandatory->isChecked();
}

const AlgorithmVariable& EditableScriptVariableTableModelEditDialog::getVariable() {
    return editable_variable;
}

void EditableScriptVariableTableModelEditDialog::on_pushButtonOk_clicked() {
    fillVariableFromUI();
    accept();
}

void EditableScriptVariableTableModelEditDialog::on_pushButtonCancel_clicked() {
    reject();
}

void EditableScriptVariableTableModelEditDialog::on_comboBoxTypes_currentTextChanged(const QString &selected_type) {
    bool enable_subtypes = (chaos::DataType::typeCodeByDescription(selected_type.toStdString()) == chaos::DataType::TYPE_BYTEARRAY);
    //set visible state of subtypes widget
    ui->widgetVariableSubtypesEditor->setVisible(enable_subtypes);
    //clear subtype if we don't need it
    if(enable_subtypes == false){editable_variable.subtype_list.clear();}
}
