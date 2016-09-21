#include "EditableSubtypeListTableModel.h"

#include<chaos/common/global.h>

EditableSubtypeListTableModel::EditableSubtypeListTableModel(QObject *parent):
    ChaosAbstractTableModel(parent),
    subtype_list_ptr(NULL){}

void EditableSubtypeListTableModel::setSubtypesList(std::vector<unsigned int> *subtype_list) {
    if(subtype_list == NULL) return;
    subtype_list_ptr = subtype_list;
}

void EditableSubtypeListTableModel::addNew() {
    CHAOS_ASSERT(subtype_list_ptr)
            beginResetModel();
    subtype_list_ptr->push_back(chaos::DataType::SUB_TYPE_BOOLEAN);
    endResetModel();
}

void EditableSubtypeListTableModel::removeAtIndex(unsigned int idx) {
    CHAOS_ASSERT(subtype_list_ptr)
            beginResetModel();
    subtype_list_ptr->erase(subtype_list_ptr->begin() + idx);
    endResetModel();
}

int EditableSubtypeListTableModel::getRowCount() const {
    return subtype_list_ptr?subtype_list_ptr->size():0;
}

int EditableSubtypeListTableModel::getColumnCount() const {
    return 2;
}

QString EditableSubtypeListTableModel::getHeaderForColumn(int column) const {
    switch(column) {
    case 0:
        return tr("Type");
    case 1:
        return tr("Unsigned");
    default:
        return QString();
    }
}

QVariant EditableSubtypeListTableModel::getCellData(int row, int column) const {
    CHAOS_ASSERT(subtype_list_ptr)
            bool is_unsigned = CHAOS_SUBTYPE_IS_UNSIGNED((*subtype_list_ptr)[row]);
    chaos::DataType::BinarySubtype bin_sub_type = (chaos::DataType::BinarySubtype)CHAOS_SUBTYPE_UNWRAP((*subtype_list_ptr)[row]);
    return QString::fromStdString(chaos::DataType::subtypeDescriptionByIndex(bin_sub_type));
}

QVariant EditableSubtypeListTableModel::getCheckeable(int row, int column) const {
    CHAOS_ASSERT(subtype_list_ptr)
            if(column == 1) {
        bool is_unsigned = CHAOS_SUBTYPE_IS_UNSIGNED((*subtype_list_ptr)[row]);
        return is_unsigned?Qt::Checked:Qt::Unchecked;
    } else {
        return QVariant();
    }
}

QVariant EditableSubtypeListTableModel::getTooltipTextForData(int row, int column) const {
    return QVariant();
}

bool EditableSubtypeListTableModel::setCellData(const QModelIndex &index, const QVariant &value) {
    switch(index.column()) {
    case 0:{
        bool is_unsigned = CHAOS_SUBTYPE_IS_UNSIGNED((*subtype_list_ptr)[index.row()]);
        //get the right code for the selcted subtype
        (*subtype_list_ptr)[index.row()] = chaos::DataType::subtypeCodeByDescription(value.toString().toStdString());
        //now reapply the unsigned flag
        if(is_unsigned) {
            CHAOS_SUBTYPE_SET_UNSIGNED((*subtype_list_ptr)[index.row()]);
        }
        break;
    }
    case 1:{
        if(value.toBool()) {
            //checked
            CHAOS_SUBTYPE_SET_UNSIGNED((*subtype_list_ptr)[index.row()]);
        }else{
            CHAOS_SUBTYPE_SET_SIGNED((*subtype_list_ptr)[index.row()]);
        }
        break;
    }
    }
    return true;
}

bool EditableSubtypeListTableModel::isCellEditable(const QModelIndex &index) const {
    return true;
}

bool EditableSubtypeListTableModel::isCellCheckable(const QModelIndex &index) const {
    return index.column() == 1;
}

QVariant EditableSubtypeListTableModel::getTextAlignForData(int row, int column) const {
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
