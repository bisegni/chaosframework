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
    }
}

void EditableDatasetTableModel::addNewElemenToToDataset(const DatasetAttribute& new_dataset_attribute) {

}

void EditableDatasetTableModel::removeElementFromDataset(const QString& attribute_name,
                                                         const DataType::DataSetAttributeIOAttribute direction) {

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

QString EditableDatasetTableModel::decodeTypeToString(chaos::DataType::DataType type) const {
    switch (type) {
    case chaos::DataType::TYPE_BOOLEAN:{
        return tr("Boolean");
    }
    case chaos::DataType::TYPE_INT32:{
        return tr("Int32");
    }
    case chaos::DataType::TYPE_INT64:{
        return tr("Int64");
    }
    case chaos::DataType::TYPE_DOUBLE:{
        return tr("Double");
    }
    case chaos::DataType::TYPE_STRING:{
        return tr("String");;
    }
    case chaos::DataType::TYPE_BYTEARRAY:{
        return tr("Binary");
    }
    default:
        return tr("UNKNOWN");
    }
}

chaos::DataType::DataType EditableDatasetTableModel::decodeStringToType(const QString& type_string) {
    if(type_string.compare("Boolean")) {
        return chaos::DataType::TYPE_BOOLEAN;
    } else if(type_string.compare("Int32")) {
        return chaos::DataType::TYPE_INT32;
    } else if(type_string.compare("Int64")) {
        return chaos::DataType::TYPE_INT64;
    } else if(type_string.compare("Double")) {
        return chaos::DataType::TYPE_BOOLEAN;
    } else if(type_string.compare("String")) {
        return chaos::DataType::TYPE_STRING;
    } else if(type_string.compare("Binary")) {
        return chaos::DataType::TYPE_BYTEARRAY;
    }
}

QVariant EditableDatasetTableModel::getCellData(int row, int column) const {
    QVariant result;
    DatasetAttribute &attribute_ref = (*attribute_list)[row];
    switch (column) {
    case 0:
        result = QString::fromStdString(attribute_ref.name);
        break;
    case 1:
        result = decodeTypeToString(attribute_ref.type);
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
        attribute_ref.type = decodeStringToType(value.toString());
        break;
    case 2:
        attribute_ref.description = QString::fromStdString(attribute_ref.description).toStdString();
        break;
    }
    return result;
}

bool EditableDatasetTableModel::isCellEditable(const QModelIndex &index) const {
    return false;
}

QVariant EditableDatasetTableModel::getTooltipTextForData(int row, int column) const {

}

QVariant EditableDatasetTableModel::getTextAlignForData(int row, int column) const {

}
