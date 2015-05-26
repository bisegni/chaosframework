#include "FixedInputChannelDatasetTableModel.h"
#include <QDebug>
using namespace chaos::common::data;

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
                vector_doe.push_back(element);
            }
        }
    }
    endResetModel();
}

int FixedInputChannelDatasetTableModel::getRowCount() const {
    return vector_doe.size();
}

int FixedInputChannelDatasetTableModel::getColumnCount() const {
    return monitoring_enabled?7:6;
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
        result = QString("Max");
        break;
    case 4:
        result = QString("Min");
        break;
    case 5:
        result = QString("Default");
        break;
    case 6:
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
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
        }
        break;
    case 4:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
        }
        break;
    case 5:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
        }
        break;
    case 6:
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
    case 3:
    case 4:
        case 5:
        result =  Qt::AlignLeft+Qt::AlignVCenter;
        break;
    case 6:
        result =  Qt::AlignHCenter+Qt::AlignVCenter;
        break;
    }

    return result;
}
