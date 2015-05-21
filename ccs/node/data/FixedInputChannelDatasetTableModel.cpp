#include "FixedInputChannelDatasetTableModel.h"
#include <QDebug>
using namespace chaos::common::data;

FixedInputChannelDatasetTableModel::FixedInputChannelDatasetTableModel(QObject *parent):
    ChaosFixedCDataWrapperTableModel(parent) {

}
FixedInputChannelDatasetTableModel::~FixedInputChannelDatasetTableModel() {

}
void FixedInputChannelDatasetTableModel::updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset) {
    //call superclas method taht will emit dataChagned
    ChaosFixedCDataWrapperTableModel::updateData(_dataset);
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
                dataset_output_element.push_back(element);
            }
        }
    }
    endResetModel();
}
int FixedInputChannelDatasetTableModel::getRowCount() const {
    return dataset_output_element.size();
}

int FixedInputChannelDatasetTableModel::getColumnCount() const {
    return 6;
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
    }
    return result;
}

QVariant FixedInputChannelDatasetTableModel::getCellData(int row, int column) const {
    QVariant result;
    QSharedPointer<CDataWrapper> element = dataset_output_element[row];
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

    case 2:{
        result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION));
        break;
    }

    case 3:{
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
        }
        break;
    }
    case 4:{
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
        }
        break;
    }
    case 5:{
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
        }
        break;
    }
    }
    return result;
}
