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

std::vector<unsigned int> DatasetAttributeReader::getSubtype() {
    std::vector<unsigned int> sub_type;
    if(!attribute_description->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE) ||
            !attribute_description->isInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE) ||
            !attribute_description->isVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE) ) return sub_type;
    //we have subtype
    if(attribute_description->isInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)) {
        //one subtype
        sub_type.push_back(attribute_description->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE));
    } else {
        std::auto_ptr<CMultiTypeDataArrayWrapper> subtype_array(attribute_description->getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE));
        for(int idx = 0;
            idx < subtype_array->size();
            idx++) {
            sub_type.push_back(subtype_array->getInt32ElementAtIndex(idx));
        }
    }
}

bool DatasetAttributeReader::isMandatory() {
    return false;
}

int DatasetAttributeReader::getDirection() {
    if(!attribute_description->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION) ||
            !attribute_description->isInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION)) return -1;

    return attribute_description->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
}
