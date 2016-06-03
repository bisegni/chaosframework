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
    switch(column) {
    case 0:{
        switch(bin_sub_type) {
        //!bool variable length
        case chaos::DataType::SUB_TYPE_BOOLEAN:
            return tr("Boolean");

            //!Integer char bit length
        case chaos::DataType::SUB_TYPE_CHAR:
            return tr("Char");

            //!Integer 8 bit length
        case chaos::DataType::SUB_TYPE_INT8:
            return tr("Int8");

            //!Integer 16 bit length
        case chaos::DataType::SUB_TYPE_INT16:
            return tr("Int16");

            //!Integer 32 bit length
        case chaos::DataType::SUB_TYPE_INT32:
            return tr("Int32");

            //!Integer 64 bit length
        case chaos::DataType::SUB_TYPE_INT64:
            return tr("Int64");

            //!Double 64 bit length
        case chaos::DataType::SUB_TYPE_DOUBLE:
            return tr("Double");

            //!C string variable length
        case chaos::DataType::SUB_TYPE_STRING:
            return tr("String");

            //! the subtype is represented by a specific mime type tagged in specific dataset constants
        case chaos::DataType::SUB_TYPE_MIME:
            return tr("Mime");

        case chaos::DataType::SUB_TYPE_NONE:
            return tr("Unset");
        }
    }
    default:
        return QString();
    }
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
    bool is_unsigned = CHAOS_SUBTYPE_IS_UNSIGNED((*dataset_subtype_list_ptr)[index.row()]);
    switch(index.column()) {
    case 0:{
        if(value.toString().compare("Boolean") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_BOOLEAN;
        } else if(value.toString().compare("Char") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_CHAR;
        } else if(value.toString().compare("Int8") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_INT8;
        } else if(value.toString().compare("Int16") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_INT16;
        } else if(value.toString().compare("Int32") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_INT32;
        } else if(value.toString().compare("Int64") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_INT64;
        } else if(value.toString().compare("Double") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_DOUBLE;
        } else if(value.toString().compare("String") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_STRING;
        } else if(value.toString().compare("Mime") == 0) {
            (*dataset_subtype_list_ptr)[index.row()] = chaos::DataType::SUB_TYPE_MIME;
        }

        //now reapply the unsigned flag
        if(is_unsigned) {
            CHAOS_SUBTYPE_SET_UNSIGNED((*dataset_subtype_list_ptr)[index.row()]);
           // = (chaos::DataType::BinarySubtype)((*dataset_subtype_list_ptr)[index.row()] | chaos::DataType::SUB_TYPE_UNSIGNED);
        }
    }
    case 1:{
        if(value.toBool()) {
            //checked
            CHAOS_SUBTYPE_SET_UNSIGNED((*dataset_subtype_list_ptr)[index.row()]);
            //(*dataset_subtype_list_ptr)[index.row()] = (chaos::DataType::BinarySubtype)((*dataset_subtype_list_ptr)[index.row()] | chaos::DataType::SUB_TYPE_UNSIGNED);
        }else{
            CHAOS_SUBTYPE_SET_SIGNED((*dataset_subtype_list_ptr)[index.row()]);
            //(*dataset_subtype_list_ptr)[index.row()] = (chaos::DataType::BinarySubtype)((*dataset_subtype_list_ptr)[index.row()] & ~chaos::DataType::SUB_TYPE_UNSIGNED);
        }
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
