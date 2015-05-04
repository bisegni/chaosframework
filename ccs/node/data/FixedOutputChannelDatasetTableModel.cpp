#include "FixedOutputChannelDatasetTableModel.h"

using namespace chaos::common::data;

FixedOutputChannelDatasetTableModel::FixedOutputChannelDatasetTableModel(const QVector< QSharedPointer<CDataWrapper> >& _dataset_output_element,
                                                                         QObject *parent):
    ChaosAbstractNodeTableModel(parent),
    dataset_output_element(_dataset_output_element) {

}

FixedOutputChannelDatasetTableModel::~FixedOutputChannelDatasetTableModel() {

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
    case 1:
        result = QString("Name");
        break;
    case 2:
        result = QString("Type");
        break;
    case 3:
        result = QString("Description");
        break;
    }
    return result;
}

QVariant FixedOutputChannelDatasetTableModel::getCellData(int row, int column) const{
    QVariant result;
    QSharedPointer<CDataWrapper> element = dataset_output_element[row];
    switch(column) {
    case 1:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)) {
            result = QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME));
        }
        break;

    case 2:
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

    case 3:
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION)) {
            return QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION));
        }
        break;
    }
    return result;
}
