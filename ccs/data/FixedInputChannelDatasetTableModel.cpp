#include "FixedInputChannelDatasetTableModel.h"

#include <QDebug>
#include <QColor>
#include <QMessageBox>

#include <boost/lexical_cast.hpp>


using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

static QColor changed_value_background_color(94,170,255);
static QColor changed_value_text_color(10,10,10);

FixedInputChannelDatasetTableModel::FixedInputChannelDatasetTableModel(const QString &node_uid,
                                                                       unsigned int dataset_type,
                                                                       QObject *parent):
    ChaosAbstractDataSetTableModel(node_uid,
                                   dataset_type,
                                   parent) {

}
FixedInputChannelDatasetTableModel::~FixedInputChannelDatasetTableModel() {

}
void FixedInputChannelDatasetTableModel::updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset) {
    //call superclas method taht will emit dataChagned
    data_wrapped = _dataset;
    int real_row = 0;
    if(data_wrapped.isNull() ||
            !data_wrapped->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)) return;
    beginResetModel();
    //reset all
    map_doe_attribute_name_index.clear();
    attribute_value_changed.clear();
    attribute_set_value.clear();
    vector_doe.clear();

    QSharedPointer<CMultiTypeDataArrayWrapper> dataset_array(data_wrapped->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION));
    for(int idx = 0;
        idx < dataset_array->size();
        idx++) {
        QSharedPointer<CDataWrapper> element(dataset_array->getCDataWrapperElementAtIndex(idx));
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION)) {
            int direction = element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
            if(direction == chaos::DataType::Input ||
                    direction == chaos::DataType::Bidirectional) {
                map_doe_attribute_name_index.insert(QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)),
                                                    QSharedPointer<AttributeInfo>(new AttributeInfo(real_row++,
                                                                                                    7,
                                                                                                    (chaos::DataType::DataType)element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE))));
                attribute_value_changed.push_back(0);
                attribute_set_value.push_back(QSharedPointer<AttributeValueChangeSet>(new AttributeValueChangeSet()));
                vector_doe.push_back(element);
            }
        }
    }
    endResetModel();
}

void FixedInputChannelDatasetTableModel::updateInstanceDescription(const QSharedPointer<CDataWrapper>& _dataset_attribute_configuration) {
    dataset_attribute_configuration.clear();
    if(_dataset_attribute_configuration.isNull() ||
            !_dataset_attribute_configuration->hasKey("attribute_value_descriptions")) return;
    QSharedPointer<CMultiTypeDataArrayWrapper> attribute_configuration(_dataset_attribute_configuration->getVectorValue("attribute_value_descriptions"));
    for(int idx = 0;
        idx < attribute_configuration->size();
        idx++){
        //fetch the attribute configuration
        QSharedPointer<CDataWrapper> attribute(attribute_configuration->getCDataWrapperElementAtIndex(idx));

        //add it to map
        QString attribute_name  = QString::fromStdString(attribute->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME));
        if(!map_doe_attribute_name_index.contains(attribute_name)) continue;
        dataset_attribute_configuration.insert(map_doe_attribute_name_index[attribute_name]->row, attribute);
    }
}

void FixedInputChannelDatasetTableModel::getAttributeChangeSet(std::vector< boost::shared_ptr<ControlUnitInputDatasetChangeSet> >& value_set_array) {
    size_t index = attribute_value_changed.find_first();

    std::vector< boost::shared_ptr<InputDatasetAttributeChangeValue> > change_set;
    while(index != boost::dynamic_bitset<>::npos) {
        QSharedPointer<CDataWrapper> element = vector_doe[index];

        boost::shared_ptr<InputDatasetAttributeChangeValue> changes(new InputDatasetAttributeChangeValue(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME),
                                                                                                         attribute_set_value[index]->current_value.toString().toStdString()));
        change_set.push_back(changes);
        index = attribute_value_changed.find_next(index);
    }
    //push this
    value_set_array.push_back( boost::shared_ptr<ControlUnitInputDatasetChangeSet>(new ControlUnitInputDatasetChangeSet(node_uid.toStdString(), change_set)));
}

void FixedInputChannelDatasetTableModel::applyChangeSet(bool commit) {
    beginResetModel();
    size_t index = attribute_value_changed.find_first();
    while(index != boost::dynamic_bitset<>::npos) {
        QSharedPointer<CDataWrapper> element = vector_doe[index];
        if(commit)
            attribute_set_value[index]->commit();
        else
            attribute_set_value[index]->reset();

        index = attribute_value_changed.find_next(index);
    }
    attribute_value_changed.reset();
    endResetModel();
}

int FixedInputChannelDatasetTableModel::getRowCount() const {
    return vector_doe.size();
}

int FixedInputChannelDatasetTableModel::getColumnCount() const {
    return monitoring_enabled?8:7;
}

QString FixedInputChannelDatasetTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch(column) {
    case 0:
        result = QString("Name");
        break;
    case 1:
        result = QString("Type");
        break;
    case 2:
        result = QString("Description");
        break;
    case 3:
        result = QString("Min");
        break;
    case 4:
        result = QString("Max");
        break;
    case 5:
        result = QString("Default");
        break;
    case 6:
        result = QString("Set value");
        break;
    case 7:
        result = QString("Current value");
        break;
    }
    return result;
}

QVariant FixedInputChannelDatasetTableModel::getCellData(int row, int column) const {
    QVariant result;
    QSharedPointer<CDataWrapper> element = vector_doe[row];
    switch(column) {
    case 0:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME));
        }
        break;

    case 1:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
            switch (element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
            case chaos::DataType::TYPE_BOOLEAN:
                result = QString("boolean");
                break;
            case chaos::DataType::TYPE_INT32:
                result = QString("Int32");
                break;
            case chaos::DataType::TYPE_INT64:
                result = QString("Int64");
                break;
            case chaos::DataType::TYPE_JSONOBJ:
                result = QString("json");
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

    case 2:
        result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION));
        break;
    case 3:
        if(dataset_attribute_configuration.contains(row) &&
                dataset_attribute_configuration[row]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
            result = QString::fromStdString(dataset_attribute_configuration[row]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE));
        } else if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE));
        }
        break;
    case 4:
        if(dataset_attribute_configuration.contains(row) &&
                dataset_attribute_configuration[row]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
            result = QString::fromStdString(dataset_attribute_configuration[row]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
        } else if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
        }
        break;
    case 5:
        if(dataset_attribute_configuration.contains(row) &&
                dataset_attribute_configuration[row]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
            result = QString::fromStdString(dataset_attribute_configuration[row]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE));
        } else if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE));
        }
        break;
    case 6:
        result = attribute_set_value[row]->current_value;
        break;
    case 7:
        if(map_doe_current_values.find(row) == map_doe_current_values.end()) {
            result = QString("...");
        } else {
            result = map_doe_current_values[row];
        }
        break;
    }
    return result;
}

QVariant FixedInputChannelDatasetTableModel::getTooltipTextForData(int row, int column) const {
    return getCellData(row, column);
}

QVariant FixedInputChannelDatasetTableModel::getTextAlignForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
    case 1:
    case 2:
        result =  Qt::AlignLeft+Qt::AlignVCenter;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        result =  Qt::AlignHCenter+Qt::AlignVCenter;
        break;

    default:
        result =  Qt::AlignLeft;
    }
    return result;
}

QVariant FixedInputChannelDatasetTableModel::getBackgroundForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        break;
    case 6:
        if(attribute_value_changed[row]){
            result = changed_value_background_color;
        }
        break;
    case 7:
        break;

    default:
        break;
    }
    return result;
}

QVariant FixedInputChannelDatasetTableModel::getTextColorForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        break;
    case 6:
        if(attribute_value_changed[row]){
            result = changed_value_text_color;
        }
        break;
    case 7:
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

#define CHECKTYPE_NUMBER(r, string, func, flag)\
    if(string.startsWith("0x")){\
    r = string.func(&flag, 16);\
    } else {\
    r = string.func(&flag);\
    }

#define CHECK_MAX_MIN_NUMERIC_TYPED_VALUE(t, tv)\
    if(dataset_attribute_configuration.contains(index.row()) &&\
    dataset_attribute_configuration[index.row()]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {\
    t min  =  boost::lexical_cast<t>(dataset_attribute_configuration[index.row()]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE));\
    if((tv < min)) {\
    error_message = QString::fromStdString(boost::str(boost::format("The value %1% is minor of %2%") % tv % min));\
    result = false;\
    break;\
    }\
    }\
    if(dataset_attribute_configuration.contains(index.row()) &&\
    dataset_attribute_configuration[index.row()]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {\
    t max  =  boost::lexical_cast<t>(dataset_attribute_configuration[index.row()]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));\
    if(tv > max) {\
    error_message = QString::fromStdString(boost::str(boost::format("The value %1% is major of %2%") % tv % max));\
    result = false;\
    break;\
    }\
    }

bool FixedInputChannelDatasetTableModel::setCellData(const QModelIndex& index, const QVariant& value) {
    bool result = true;
    QString error_message;

    QSharedPointer<CDataWrapper> element = vector_doe[index.row()];
    switch (element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
    case chaos::DataType::TYPE_BOOLEAN:{
        CHECKTYPE(result, bool, value)
                if(!result) {
            error_message = tr("The value is not convertible to double");
        }
        break;
    }
    case chaos::DataType::TYPE_INT32:{
        int32_t typed_value;
        CHECKTYPE_NUMBER(typed_value, value.toString(), toInt, result);
        if(!result) {
            error_message = tr("The value is not convertible to int32");
            break;
        }
        if(dataset_attribute_configuration.contains(index.row()) &&
                dataset_attribute_configuration[index.row()]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
            int32_t min;
            CHECKTYPE_NUMBER(min, QString::fromStdString(dataset_attribute_configuration[index.row()]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)), toInt, result);
            if(result &&
                    (typed_value < min)) {
                error_message = QString::fromStdString(boost::str(boost::format("The value %1% is minor of %2%") % typed_value % min));
                result = false;
                break;
            }
        }
        if(dataset_attribute_configuration.contains(index.row()) &&
                dataset_attribute_configuration[index.row()]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
            int32_t max;
            CHECKTYPE_NUMBER(max, QString::fromStdString(dataset_attribute_configuration[index.row()]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)), toInt, result);
            if(result &&
                    (typed_value > max)) {
                error_message = QString::fromStdString(boost::str(boost::format("The value %1% is major of %2%") % typed_value % max));
                result = false;
                break;
            }
        }
        break;
    }
    case chaos::DataType::TYPE_INT64:{
        int64_t typed_value;
        CHECKTYPE_NUMBER(typed_value, value.toString(), toLongLong, result);

        qDebug() << "FixedInputChannelDatasetTableModel::setCellData str "<<value.toString() << " value:" <<typed_value;

        if(!result) {
            error_message = tr("The value is not convertible to int64_t");
            break;
        }
        if(dataset_attribute_configuration.contains(index.row())) {
            if(dataset_attribute_configuration[index.row()]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
                int64_t min;
                QString min_value_string = QString::fromStdString(dataset_attribute_configuration[index.row()]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE));
                CHECKTYPE_NUMBER(min, min_value_string, toLongLong, result);
                qDebug() << "FixedInputChannelDatasetTableModel::setCellData min str" << min_value_string << " value:" <<min;
                if(result &&
                        (typed_value < min)) {
                    error_message = QString::fromStdString(boost::str(boost::format("The value %1% is minor of %2%") % typed_value % min));\
                    result = false;
                    break;
                }
            }

            if(dataset_attribute_configuration[index.row()]->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
                int64_t max;
                QString max_value_string = QString::fromStdString(dataset_attribute_configuration[index.row()]->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
                CHECKTYPE_NUMBER(max, max_value_string, toLongLong, result);
                qDebug() << "FixedInputChannelDatasetTableModel::setCellData max str" << max_value_string << " value:" <<max;
                if(result &&
                        (typed_value > max)) {
                    error_message = QString::fromStdString(boost::str(boost::format("The value %1% is major of %2%") % typed_value % max));
                    result = false;
                    break;
                }
            }
        }
        break;
    }
    case chaos::DataType::TYPE_DOUBLE:{
        CHECKTYPE(result, double, value)
                if(!result) {
            error_message = tr("The value is not convertible to double");
            break;
        }
        CHECK_MAX_MIN_NUMERIC_TYPED_VALUE(double, typed_value)
                break;
    }
        case chaos::DataType::TYPE_JSONOBJ:
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
        msgBox.setText(tr("Dataset Cast Error"));
        msgBox.setInformativeText(error_message);
        msgBox.exec();
    } else {
        attribute_value_changed[index.row()] = true;
        attribute_set_value[index.row()]->setCurrentValue(value);
    }
    return result;
}

bool FixedInputChannelDatasetTableModel::isCellEditable(const QModelIndex &index) const {
    return index.column() == 6;
}
