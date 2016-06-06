#include "EditableDatasetTableModelEditDialog.h"
#include "ui_EditableDatasetTableModelEditDialog.h"

EditableDatasetTableModelEditDialog::EditableDatasetTableModelEditDialog(QWidget *parent) :
    QDialog(parent),
    editable_dataset_attribute(),
    ui(new Ui::EditableDatasetTableModelEditDialog){
    ui->setupUi(this);
    ui->lineEditAttributeName->setFocus();
    //init widget for subtype
    ui->widgetAttributeSubtypesEditor->setVisible(false);
    ui->widgetAttributeSubtypesEditor->setDatasetAttrbiute(&editable_dataset_attribute);
}

EditableDatasetTableModelEditDialog::~EditableDatasetTableModelEditDialog() {
    delete ui;
}

void EditableDatasetTableModelEditDialog::setDatsetAttribute(const chaos::service_common::data::dataset::DatasetAttribute& dataset_attribute) {
    editable_dataset_attribute = dataset_attribute;
    ui->widgetAttributeSubtypesEditor->setDatasetAttrbiute(&editable_dataset_attribute);
    fillAttributeFromUI();
}

void EditableDatasetTableModelEditDialog::fillUIFromAttribute() {
    ui->lineEditAttributeName->setText(QString::fromStdString(editable_dataset_attribute.name));
    ui->plainTextEditAttributeDescription->setPlainText(QString::fromStdString(editable_dataset_attribute.description));
    ui->comboBoxTypes->setCurrentIndex(editable_dataset_attribute.type);
    ui->comboBoxAttributeDirection->setCurrentIndex(editable_dataset_attribute.direction);
}

void EditableDatasetTableModelEditDialog::fillAttributeFromUI() {
    editable_dataset_attribute.name = ui->lineEditAttributeName->text().toStdString();
    editable_dataset_attribute.description = ui->plainTextEditAttributeDescription->document()->toPlainText().toStdString();
    editable_dataset_attribute.type = (chaos::DataType::DataType)ui->comboBoxTypes->currentIndex();
    editable_dataset_attribute.direction = (chaos::DataType::DataSetAttributeIOAttribute)ui->comboBoxAttributeDirection->currentIndex();
}

const chaos::service_common::data::dataset::DatasetAttribute& EditableDatasetTableModelEditDialog::getDatasetAttribute() {
    return editable_dataset_attribute;
}

void EditableDatasetTableModelEditDialog::on_pushButtonOk_clicked() {
    fillAttributeFromUI();
    accept();
}

void EditableDatasetTableModelEditDialog::on_pushButtonCancel_clicked() {
    reject();
}

void EditableDatasetTableModelEditDialog::on_comboBoxTypes_currentTextChanged(const QString &selected_type) {
    bool enable_subtypes = (chaos::DataType::typeCodeByDescription(selected_type.toStdString()) == chaos::DataType::TYPE_BYTEARRAY);
    //set visible state of subtypes widget
    ui->widgetAttributeSubtypesEditor->setVisible(enable_subtypes);
    //clear subtype if we don't need it
    if(enable_subtypes == false){editable_dataset_attribute.binary_subtype_list.clear();}
}
