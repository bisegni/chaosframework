#include "EditableDatasetTableModelEditDialog.h"
#include "ui_EditableDatasetTableModelEditDialog.h"

EditableDatasetTableModelEditDialog::EditableDatasetTableModelEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditableDatasetTableModelEditDialog),
    editable_dataset_attribute(){
    ui->setupUi(this);
    ui->widgetAttributeSubtypesEditor->setDatasetAttrbiute(&editable_dataset_attribute);
}

EditableDatasetTableModelEditDialog::~EditableDatasetTableModelEditDialog() {
    delete ui;
}

void EditableDatasetTableModelEditDialog::setDatsetAttribute(const chaos::service_common::data::dataset::DatasetAttribute& dataset_attribute) {
    editable_dataset_attribute = dataset_attribute;
    ui->widgetAttributeSubtypesEditor->setDatasetAttrbiute(&editable_dataset_attribute);
}

void EditableDatasetTableModelEditDialog::fillUIFromAttribute() {
    ui->lineEditAttributeName->setText(QString::fromStdString(editable_dataset_attribute.name));
    ui->plainTextEditAttributeDescription->setPlainText(QString::fromStdString(editable_dataset_attribute.description));
}

void EditableDatasetTableModelEditDialog::fillAttributeFromUI() {
    editable_dataset_attribute.name = ui->lineEditAttributeName->text().toStdString();
    editable_dataset_attribute.description = ui->plainTextEditAttributeDescription->document()->toPlainText().toStdString();
}

const chaos::service_common::data::dataset::DatasetAttribute& EditableDatasetTableModelEditDialog::getDatasetAttribute() {
    return editable_dataset_attribute;
}

void EditableDatasetTableModelEditDialog::on_pushButtonOk_clicked() {
    accept();
}

void EditableDatasetTableModelEditDialog::on_pushButtonCancel_clicked() {
    reject();
}
