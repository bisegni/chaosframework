#include "FixedOutputChannelDatasetTableModel.h"

#include <QDateTime>

using namespace chaos::common::data;

FixedOutputChannelDatasetTableModel::FixedOutputChannelDatasetTableModel(const QString& node_uid,
                                                                         unsigned int dataset_type,
                                                                         QObject *parent):
    ChaosAbstractDataSetTableModel(node_uid,
                                   dataset_type,
                                   parent) {

}

FixedOutputChannelDatasetTableModel::~FixedOutputChannelDatasetTableModel() {

}

void FixedOutputChannelDatasetTableModel::updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset) {
    //call superclas method taht will emit dataChagned
    data_wrapped = _dataset;
    int real_row = 0;
    if(data_wrapped.isNull() ||
            !data_wrapped->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)) return;
    beginResetModel();

    //add the default timestamp field
    map_doe_attribute_name_index.insert(chaos::DataPackCommonKey::DPCK_TIMESTAMP,
                                        QSharedPointer<AttributeInfo>(new AttributeInfo(real_row++,
                                                                                        4,
                                                                                        chaos::DataType::TYPE_INT64)));
    //add other output channels
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
            if(direction == chaos::DataType::Output ||
                    direction == chaos::DataType::Bidirectional) {
                //create index for the current value of this element
                map_doe_attribute_name_index.insert(QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)),
                                                    QSharedPointer<AttributeInfo>(new AttributeInfo(real_row++,
                                                                                                    4,
                                                                                                    (chaos::DataType::DataType)element->getInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE))));
                vector_doe.push_back(element);
            }
        }
    }
    endResetModel();
}

int FixedOutputChannelDatasetTableModel::getRowCount() const {
    return vector_doe.size()+1;
}

int FixedOutputChannelDatasetTableModel::getColumnCount() const {
    return monitoring_enabled?4:3;
}

QString FixedOutputChannelDatasetTableModel::getHeaderForColumn(int column) const {
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
        result = QString("Current value");
        break;
    }
    return result;
}

QVariant FixedOutputChannelDatasetTableModel::getCellData(int row, int column) const {
    QVariant result;
     if(row == 0) {
        switch(column) {
        case 0:
               result = chaos::DataPackCommonKey::DPCK_TIMESTAMP;
            break;

        case 1:
            result = QString("Int64");
            break;

        case 2:
            result = QString("Publishing Timestamp");
            break;
        case 3:
            if(map_doe_current_values.find(row) == map_doe_current_values.end()) {
                result = QString("...");
            } else {
                result = QDateTime::fromMSecsSinceEpoch(map_doe_current_values[row].toULongLong(), Qt::LocalTime).toString();
            }
            break;
        }
    } else {
        QSharedPointer<CDataWrapper> element = vector_doe[row-1];
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
                case chaos::DataType::TYPE_CLUSTER:
                    result = QString("Cluster");
                    break;
                default:
                    break;
                }
            }
            break;

        case 2:
            if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION)) {
                return QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION));
            }
            break;
        case 3:
            if(map_doe_current_values.find(row) == map_doe_current_values.end()) {
                result = QString("...");
            } else {
                result = map_doe_current_values[row];
            }
            break;
        }
    }
    return result;
}

QVariant FixedOutputChannelDatasetTableModel::getTooltipTextForData(int row, int column) const {
    return getCellData(row, column);
}

QVariant FixedOutputChannelDatasetTableModel::getTextAlignForData(int row, int column) const {
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
    }

    return result;
}
