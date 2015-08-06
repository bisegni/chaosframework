#include "DatasetAttributeReader.h"
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;

DatasetAttributeReader::DatasetAttributeReader(QSharedPointer<CDataWrapper> _attribute_description,
                                               QObject *parent) :
    AttributeReader(parent),
    attribute_description(_attribute_description) {

}

QString DatasetAttributeReader::getName() {
    if(!attribute_description->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME) ||
            !attribute_description->isStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)) return QString();

    return QString::fromStdString(attribute_description->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME));
}

QString DatasetAttributeReader::getDescription() {
    if(!attribute_description->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION) ||
            !attribute_description->isStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION)) return QString();

    return QString::fromStdString(attribute_description->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION));
}

int32_t DatasetAttributeReader::getType() {
    if(!attribute_description->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE) ||
            !attribute_description->isInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) return -1;

    return attribute_description->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE);
}

bool DatasetAttributeReader::isMandatory() {
    return false;
}

int DatasetAttributeReader::getDirection() {
    if(!attribute_description->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION) ||
            !attribute_description->isInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION)) return -1;

    return attribute_description->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
}
