#include "EditableDatasetTableModel.h"
#include "editor/dataset/EditableDatasetTableModelEditDialog.h"
using namespace chaos;
using namespace chaos::service_common::data::dataset;

EditableDatasetTableModel::EditableDatasetTableModel(QObject *parent):
    ChaosAbstractTableModel(parent),
    attribute_list(NULL){}

void EditableDatasetTableModel::addNewDatasetAttribute() {
    EditableDatasetTableModelEditDialog new_attribute_dialog;
    if(new_attribute_dialog.exec() == QDialog::Accepted) {
        //we have a new element to add
        addNewElemenToToDataset(new_attribute_dialog.getDatasetAttribute());
    }
}

void EditableDatasetTableModel::addNewElemenToToDataset(const DatasetAttribute& new_dataset_attribute) {
    if(!attribute_list) return;
    beginResetModel();
    attribute_list->push_back(new_dataset_attribute);
    endResetModel();
}

void EditableDatasetTableModel::removeElementFromDataset(const QString& attribute_name,
                                                         const DataType::DataSetAttributeIOAttribute direction) {

}

void EditableDatasetTableModel::editDatasetAttributeAtIndex(unsigned int dsattr_index) {
    if(dsattr_index >= (*attribute_list).size()) return;

    EditableDatasetTableModelEditDialog new_attribute_dialog;
    new_attribute_dialog.setDatsetAttribute((*attribute_list)[dsattr_index]);
    if(new_attribute_dialog.exec() == QDialog::Accepted) {
        //we have a new element to add
        beginResetModel();
        (*attribute_list)[dsattr_index] = new_attribute_dialog.getDatasetAttribute();
        endResetModel();
    }
}

void EditableDatasetTableModel::setDatasetAttributeList(chaos::service_common::data::dataset::DatasetAttributeList *master_attribute_list) {
    beginResetModel();
    attribute_list = master_attribute_list;
    endResetModel();
}

int EditableDatasetTableModel::getRowCount() const {
    return (attribute_list?attribute_list->size():0);
}

int EditableDatasetTableModel::getColumnCount() const {
    return 3;
}

QString EditableDatasetTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch (column) {
    case 0:
        result = QString("Name");
        break;
    case 1:
        result = QString("Type");
        break;
    case 2:
        result = QString("Description");
        break;
    }
    return result;
}


QVariant EditableDatasetTableModel::getCellData(int row, int column) const {
    QVariant result;
    DatasetAttribute &attribute_ref = (*attribute_list)[row];
    switch (column) {
    case 0:
        result = QString::fromStdString(attribute_ref.name);
        break;
    case 1:
        result =  QString::fromStdString(chaos::DataType::typeDescriptionByCode(attribute_ref.type));
        break;
    case 2:
        result = QString::fromStdString(attribute_ref.description);
        break;
    }
    return result;
}

bool EditableDatasetTableModel::setCellData(const QModelIndex &index, const QVariant &value) {
    bool result = true;
    DatasetAttribute &attribute_ref =  (*attribute_list)[index.row()];
    switch (index.column()) {
    case 0:
        attribute_ref.name = value.toString().toStdString();
        break;
    case 1:
        attribute_ref.type = chaos::DataType::typeCodeByDescription(value.toString().toStdString());
        break;
    case 2:
        attribute_ref.description = value.toString().toStdString();
        break;
    }
    return result;
}

bool EditableDatasetTableModel::isCellEditable(const QModelIndex &index) const {
    return false;
}

QVariant EditableDatasetTableModel::getTooltipTextForData(int row, int column) const {
    return getCellData(row, column);
}

QVariant EditableDatasetTableModel::getTextAlignForData(int row, int column) const {
    return QVariant();
}
