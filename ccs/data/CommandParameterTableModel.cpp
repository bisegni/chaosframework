#include "CommandParameterTableModel.h"

#include <QDebug>
#include <QColor>
#include <QMessageBox>

#include <boost/lexical_cast.hpp>


using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

static QColor changed_value_background_color_mandatory(94,170,255);
static QColor changed_value_text_color_mandatory(10,10,10);


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
    attribute_changes.clear();
    CMultiTypeDataArrayWrapperSPtr attribute_array = command_description->getVectorValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS);
    for(int idx = 0;
        idx < attribute_array->size();
        idx++) {
        QSharedPointer<CDataWrapper> attribute(attribute_array->getCDataWrapperElementAtIndex(idx));
        if(attribute->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME) &&
                attribute->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION) &&
                attribute->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE) &&
                attribute->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG)){

            QSharedPointer<AttributeValueChangeSet> attribute_change_set(new AttributeValueChangeSet());
            attribute_change_set->attribute_raw_description = attribute;
            attribute_change_set->attribute_name = QString::fromStdString(attribute->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME));
            attribute_change_set->is_mandatory = ((attribute->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG) &
                                                   BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)==
                                                  BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY);
            attribute_change_set->type = attribute->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE);
            attribute_changes.push_back(attribute_change_set);
        }
    }
    endResetModel();
}

void CommandParameterTableModel::fillTemplate(chaos::metadata_service_client::api_proxy::node::CommandTemplate &command_template) {
    foreach (QSharedPointer<AttributeValueChangeSet> attribute, attribute_changes) {
        ChaosSharedPtr<CDataWrapperKeyValueSetter> kv_setter;
        if(attribute->parametrize) {
            kv_setter = ChaosSharedPtr<CDataWrapperKeyValueSetter>(new CDataWrapperNullKeyValueSetter(attribute->attribute_name.toStdString()));
        } else if(!attribute->current_value.isNull()) {
            switch (attribute->type) {
            case chaos::DataType::TYPE_BOOLEAN:
                kv_setter = ChaosSharedPtr<CDataWrapperKeyValueSetter>(new CDataWrapperBoolKeyValueSetter(attribute->attribute_name.toStdString(),
                                                                                                             attribute->current_value.toBool()));
                break;
            case chaos::DataType::TYPE_INT32:
                kv_setter = ChaosSharedPtr<CDataWrapperKeyValueSetter>(new CDataWrapperInt32KeyValueSetter(attribute->attribute_name.toStdString(),
                                                                                                              attribute->current_value.toInt()));
                break;
            case chaos::DataType::TYPE_INT64:
                kv_setter = ChaosSharedPtr<CDataWrapperKeyValueSetter>(new CDataWrapperInt64KeyValueSetter(attribute->attribute_name.toStdString(),
                                                                                                              attribute->current_value.toLongLong()));
                break;
                
                case chaos::DataType::TYPE_CLUSTER:
            case chaos::DataType::TYPE_STRING:
                kv_setter = ChaosSharedPtr<CDataWrapperKeyValueSetter>(new CDataWrapperStringKeyValueSetter(attribute->attribute_name.toStdString(),
                                                                                                               attribute->current_value.toString().toStdString()));
                break;
            case chaos::DataType::TYPE_DOUBLE:
                kv_setter = ChaosSharedPtr<CDataWrapperKeyValueSetter>(new CDataWrapperDoubleKeyValueSetter(attribute->attribute_name.toStdString(),
                                                                                                               attribute->current_value.toDouble()));
                break;
            case chaos::DataType::TYPE_BYTEARRAY:
                break;
            default:

                break;
            }
        }
        if(kv_setter.get()) {
            command_template.parameter_value_list.push_back(kv_setter);
        }
    }
}

void CommandParameterTableModel::applyTemplate(const QSharedPointer<chaos::common::data::CDataWrapper>& command_template) {
    beginResetModel();
    foreach (QSharedPointer<AttributeValueChangeSet> attribute, attribute_changes) {
        const std::string attribute_name = attribute->attribute_name.toStdString();
        if(command_template->hasKey(attribute_name)) {
            attribute->parametrize = command_template->isNullValue(attribute_name);
            if(attribute->parametrize == false) {
                switch (attribute->type) {
                case chaos::DataType::TYPE_BOOLEAN:
                    attribute->current_value = command_template->getBoolValue(attribute_name);
                    break;
                case chaos::DataType::TYPE_INT32:
                    attribute->current_value = command_template->getInt32Value(attribute_name);
                    break;
                case chaos::DataType::TYPE_INT64:
                    attribute->current_value = (qlonglong)command_template->getInt64Value(attribute_name);
                    break;
                case chaos::DataType::TYPE_CLUSTER:
                case chaos::DataType::TYPE_STRING:
                 
                    attribute->current_value = QString::fromStdString(command_template->getStringValue(attribute_name));
                    break;
                case chaos::DataType::TYPE_DOUBLE:
                    attribute->current_value = command_template->getDoubleValue(attribute_name);
                    break;
                case chaos::DataType::TYPE_BYTEARRAY:
                    break;
                default:
                    break;
                }
            }
        }
    }
    endResetModel();
}

bool CommandParameterTableModel::validation(QString& param_in_error) {
    bool result = true;
    foreach (QSharedPointer<AttributeValueChangeSet> attribute, attribute_changes) {
        result = (attribute->is_mandatory?(attribute->parametrize || !attribute->current_value.isNull()):true);
        if(!result){
            param_in_error = attribute->attribute_name;
            break;
        }
    }
    return result;
}

void CommandParameterTableModel::resetChanges() {
    beginResetModel();
    foreach (QSharedPointer<AttributeValueChangeSet> attribute_change,  attribute_changes) {
        attribute_change->reset();
    }
    endResetModel();
}

int CommandParameterTableModel::getRowCount() const {
    return attribute_changes.size();
}

int CommandParameterTableModel::getColumnCount() const {
    return 5;
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
    case 4:
        result = QString("Parametrize");
        break;
    }
    return result;
}

QVariant CommandParameterTableModel::getCellData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
        result = attribute_changes[row]->attribute_name;
        break;
    case 1:
        result = QString::fromStdString(attribute_changes[row]->attribute_raw_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION));
        break;
    case 2:
        switch (attribute_changes[row]->type) {
        case chaos::DataType::TYPE_BOOLEAN:
            result = QString("boolean");
            break;
        case chaos::DataType::TYPE_INT32:
            result = QString("Int32");
            break;
        case chaos::DataType::TYPE_INT64:
            result = QString("Int64");
            break;
        case chaos::DataType::TYPE_CLUSTER:
             result = QString("cluster");
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
        break;
    case 3:
        result = attribute_changes[row]->current_value;
        break;
    default:
        break;
    }
    return result;
}

QVariant CommandParameterTableModel::getCheckeable(int row, int column) const {
    QVariant result;
    switch(column){
    case 4:
        result = attribute_changes[row]->parametrize?Qt::Checked:Qt::Unchecked;
        break;

    default:
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
    case 4:
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
    case 4:
        if(attribute_changes[row]->is_mandatory) {
            result = changed_value_background_color_mandatory;
        }
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
        if(attribute_changes[row]->is_mandatory) {
            result = changed_value_text_color_mandatory;
        }
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
    if(index.column() == 3) {
        QString error_message;

        if(attribute_changes[index.row()]->parametrize) {
            QMessageBox msgBox;
            msgBox.setText(tr("Command Attribute Cast Error"));
            msgBox.setInformativeText(tr("parametrized attribute can't be valorized"));
            msgBox.exec();
            result = false;
        } else{

            switch (attribute_changes[index.row()]->type) {
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
                }
                break;

            }
            case chaos::DataType::TYPE_INT64:{
                CHECKTYPE(result, int64_t, value)
                        if(!result) {
                    error_message = tr("The value is not convertible to int64_t");
                }
                break;

            }
            case chaos::DataType::TYPE_DOUBLE:{
                CHECKTYPE(result, double, value)
                        if(!result) {
                    error_message = tr("The value is not convertible to double");
                }
                break;

            }
            case chaos::DataType::TYPE_CLUSTER:{
                CDataWrapper tmp;
                try {
                    tmp.setSerializedJsonData(value.toString().toStdString().c_str());
                    result = true;
                } catch(...){
                    result = false;
                     error_message = tr("The value is not convertible to a json string");

                }
                break;
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
                attribute_changes[index.row()]->setCurrentValue(value);
            }
        }
    }else if(index.column() == 4) {
        attribute_changes[index.row()]->parametrize = value.toBool();
        result = true;
    }
    return result;
}

bool CommandParameterTableModel::isCellEditable(const QModelIndex &index) const {
    return index.column() == 3 || index.column() == 4;
}

bool CommandParameterTableModel::isCellCheckable(const QModelIndex &index) const {
    return index.column() == 4;
}
