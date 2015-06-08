#include "CommandParameterTableModel.h"

#include <QDebug>
#include <QColor>
#include <QMessageBox>

#include <boost/lexical_cast.hpp>


using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

static QColor changed_value_background_color(94,170,255);
static QColor changed_value_text_color(10,10,10);


CommandParameterTableModel::CommandParameterTableModel(QObject *parent):
    ChaosAbstractTableModel(parent) {}

CommandParameterTableModel::~CommandParameterTableModel() {}

void CommandParameterTableModel::updateAttribute(const QSharedPointer<chaos::common::data::CDataWrapper>& command_description) {
    //call superclas method taht will emit dataChagned
    int real_row = 0;
    if(command_description.isNull() ||
            !command_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS)) return;
    beginResetModel();
    //remove old attribute list
    command_attribute_description.clear();

    QSharedPointer<CMultiTypeDataArrayWrapper> attribute_array(command_description->getVectorValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS));
    for(int idx = 0;
        idx < attribute_array->size();
        idx++) {
        QSharedPointer<CDataWrapper> attribute(attribute_array->getCDataWrapperElementAtIndex(idx));
        if(attribute->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME) ||
                attribute->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION) ||
                attribute->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)){
            //add well formed attribute
            command_attribute_description.push_back(attribute);
        }
    }
    endResetModel();
}

int CommandParameterTableModel::getRowCount() const {
    return command_attribute_description.size();
}

int CommandParameterTableModel::getColumnCount() const {
    return 4;
}

QString CommandParameterTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch(column) {
    case 0:
        result = QString("Name");
        break;
    case 1:
        result = QString("Description");
        break;
    case 2:
        result = QString("Type");
        break;
    case 3:
        result = QString("Set value");
        break;
    }
    return result;
}

QVariant CommandParameterTableModel::getCellData(int row, int column) const {
    QVariant result;
    QSharedPointer<CDataWrapper> element = command_attribute_description[row];
    switch(column) {
    case 0:
        if(element->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME)) {
            result = QString::fromStdString(element->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME));
        }
        break;
    case 1:
        if(element->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION)) {
            result = QString::fromStdString(element->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION));
        }
        break;
    case 2:
        if(element->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)) {
            switch (element->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)) {
            case chaos::DataType::TYPE_BOOLEAN:
                result = QString("boolean");
                break;
            case chaos::DataType::TYPE_INT32:
                result = QString("Int32");
                break;
            case chaos::DataType::TYPE_INT64:
                result = QString("Int64");
                break;
            case chaos::DataType::TYPE_STRING:
                result = QString("String");
                break;
            case chaos::DataType::TYPE_DOUBLE:
                result = QString("Double");
                break;
            case chaos::DataType::TYPE_BYTEARRAY:
                result = QString("Binary");
                break;
            default:
                result = QString("------");
                break;
            }
        }
        break;
    case 3:
        break;
    }
    return result;
}

QVariant CommandParameterTableModel::getTooltipTextForData(int row, int column) const {
    return getCellData(row, column);
}

QVariant CommandParameterTableModel::getTextAlignForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
    case 1:
    case 2:
        result =  Qt::AlignLeft+Qt::AlignVCenter;
        break;
    case 3:
        result =  Qt::AlignHCenter+Qt::AlignVCenter;
        break;

    default:
        result =  Qt::AlignLeft;
    }
    return result;
}

QVariant CommandParameterTableModel::getBackgroundForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
    case 1:
    case 2:
    case 3:
        break;
    default:
        break;
    }
    return result;
}

QVariant CommandParameterTableModel::getTextColorForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
    case 1:
    case 2:
    case 3:
        break;

    default:
        break;
    }
    return result;
}

#define CHECKTYPE(r, t, v)\
    t typed_value;\
    try{\
    typed_value = boost::lexical_cast<t>(v.toString().toStdString());\
    r=true;\
    }catch(...){r=false;}


bool CommandParameterTableModel::setCellData(const QModelIndex& index, const QVariant& value) {
    bool result = true;
    QString error_message;

    QSharedPointer<CDataWrapper> element = command_attribute_description[index.row()];
    switch (element->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)) {
    case chaos::DataType::TYPE_BOOLEAN:{
        CHECKTYPE(result, bool, value)
                if(!result) {
            error_message = tr("The value is not convertible to double");
        }
        break;
    }
    case chaos::DataType::TYPE_INT32:{
        CHECKTYPE(result, int32_t, value)
                if(!result) {
            error_message = tr("The value is not convertible to int32");
            break;
        }
    }
    case chaos::DataType::TYPE_INT64:{
        CHECKTYPE(result, int64_t, value)
                if(!result) {
            error_message = tr("The value is not convertible to int64_t");
            break;
        }
    }
    case chaos::DataType::TYPE_DOUBLE:{
        CHECKTYPE(result, double, value)
                if(!result) {
            error_message = tr("The value is not convertible to double");
            break;
        }
    }
    case chaos::DataType::TYPE_STRING:{
        CHECKTYPE(result, std::string, value)
        if(!result) {
            error_message = tr("The value is not convertible to std::string");
        }
        break;
    }
    default:
        break;
    }
    if(!result){
        QMessageBox msgBox;
        msgBox.setText(tr("Command Attribute Cast Error"));
        msgBox.setInformativeText(error_message);
        msgBox.exec();
    } else {
       // attribute_value_changed[index.row()] = true;
       // attribute_set_value[index.row()]->setCurrentValue(value);
    }
    return result;
}

bool CommandParameterTableModel::isCellEditable(const QModelIndex &index) const {
    return index.column() == 3;
}
