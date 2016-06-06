#include "DatasAttributeSubtypeTableModel.h"

DatasAttributeSubtypeTableModel::DatasAttributeSubtypeTableModel(QObject *parent):
    ChaosAbstractTableModel(parent),
    dataset_subtype_list_ptr(NULL){}

void DatasAttributeSubtypeTableModel::setDatasetSubtypesList(chaos::service_common::data::dataset::DatasetSubtypeList *dataset_subtype_list) {
    if(dataset_subtype_list == NULL) return;
    dataset_subtype_list_ptr = dataset_subtype_list;
}

void DatasAttributeSubtypeTableModel::addNew() {
    CHAOS_ASSERT(dataset_subtype_list_ptr)
            beginResetModel();
    dataset_subtype_list_ptr->push_back(chaos::DataType::SUB_TYPE_BOOLEAN);
    endResetModel();
}

void DatasAttributeSubtypeTableModel::removeAtIndex(unsigned int idx) {
    CHAOS_ASSERT(dataset_subtype_list_ptr)
            beginResetModel();
    dataset_subtype_list_ptr->erase(dataset_subtype_list_ptr->begin() + idx);
    endResetModel();
}

int DatasAttributeSubtypeTableModel::getRowCount() const {
    return dataset_subtype_list_ptr?dataset_subtype_list_ptr->size():0;
}

int DatasAttributeSubtypeTableModel::getColumnCount() const {
    return 2;
}

QString DatasAttributeSubtypeTableModel::getHeaderForColumn(int column) const {
    switch(column) {
    case 0:
        return tr("Type");
    case 1:
        return tr("Unsigned");
    default:
        return QString();
    }
}

QVariant DatasAttributeSubtypeTableModel::getCellData(int row, int column) const {
    CHAOS_ASSERT(dataset_subtype_list_ptr)
    bool is_unsigned = CHAOS_SUBTYPE_IS_UNSIGNED((*dataset_subtype_list_ptr)[row]);
    chaos::DataType::BinarySubtype bin_sub_type = (chaos::DataType::BinarySubtype)CHAOS_SUBTYPE_UNWRAP((*dataset_subtype_list_ptr)[row]);
    return QString::fromStdString(chaos::DataType::subtypeDescriptionByIndex(bin_sub_type));
}

QVariant DatasAttributeSubtypeTableModel::getCheckeable(int row, int column) const {
    CHAOS_ASSERT(dataset_subtype_list_ptr)
            if(column == 1) {
        bool is_unsigned = CHAOS_SUBTYPE_IS_UNSIGNED((*dataset_subtype_list_ptr)[row]);
        return is_unsigned?Qt::Checked:Qt::Unchecked;
    } else {
        return QVariant();
    }
}

QVariant DatasAttributeSubtypeTableModel::getTooltipTextForData(int row, int column) const {
    return QVariant();
}

bool DatasAttributeSubtypeTableModel::setCellData(const QModelIndex &index, const QVariant &value) {
    switch(index.column()) {
    case 0:{
        bool is_unsigned = CHAOS_SUBTYPE_IS_UNSIGNED((*dataset_subtype_list_ptr)[index.row()]);
        //get the right code for the selcted subtype
         (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::subtypeCodeByDescription(value.toString().toStdString());
        //now reapply the unsigned flag
        if(is_unsigned) {
            CHAOS_SUBTYPE_SET_UNSIGNED((*dataset_subtype_list_ptr)[index.row()]);
        }
        break;
    }
    case 1:{
        if(value.toBool()) {
            //checked
            CHAOS_SUBTYPE_SET_UNSIGNED((*dataset_subtype_list_ptr)[index.row()]);
        }else{
            CHAOS_SUBTYPE_SET_SIGNED((*dataset_subtype_list_ptr)[index.row()]);
        }
        break;
    }
    }
    return true;
}

bool DatasAttributeSubtypeTableModel::isCellEditable(const QModelIndex &index) const {
    return true;
}

bool DatasAttributeSubtypeTableModel::isCellCheckable(const QModelIndex &index) const {
    return index.column() == 1;
}

QVariant DatasAttributeSubtypeTableModel::getTextAlignForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
        result =  Qt::AlignLeft+Qt::AlignVCenter;
        break;
    case 1:
        result =  Qt::AlignCenter+Qt::AlignVCenter;
        break;
    }
    return result;
}
