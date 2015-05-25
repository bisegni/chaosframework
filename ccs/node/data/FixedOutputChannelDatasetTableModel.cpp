#include "FixedOutputChannelDatasetTableModel.h"

using namespace chaos::common::data;

FixedOutputChannelDatasetTableModel::FixedOutputChannelDatasetTableModel(QObject *parent):
    ChaosFixedCDataWrapperTableModel(parent) {

}

FixedOutputChannelDatasetTableModel::~FixedOutputChannelDatasetTableModel() {

}

void FixedOutputChannelDatasetTableModel::updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset) {
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
            if(direction == chaos::DataType::Output ||
                    direction == chaos::DataType::Bidirectional) {
                dataset_output_element.push_back(element);
            }
        }
    }
    endResetModel();
}

int FixedOutputChannelDatasetTableModel::getRowCount() const {
    return dataset_output_element.size();
}

int FixedOutputChannelDatasetTableModel::getColumnCount() const {
    return 3;
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
    }
    return result;
}

QVariant FixedOutputChannelDatasetTableModel::getCellData(int row, int column) const{
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
    }
    return result;
}

QVariant FixedOutputChannelDatasetTableModel::getTooltipTextForData(int row, int column) const {
    return getCellData(row, column);
}
