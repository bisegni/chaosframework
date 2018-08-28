#include "DatasetReader.h"


using namespace chaos::common::data;
using namespace chaos::common::batch_command;

DatasetReader::DatasetReader(QSharedPointer<chaos::common::data::CDataWrapper> _dataset_description,
                             QObject *parent) :
    QObject(parent),
    dataset_description(_dataset_description) {
    //we need to find and fill cache
    //add other output channels
    CMultiTypeDataArrayWrapperSPtr dataset_array = dataset_description->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    for(int idx = 0;
        idx < dataset_array->size();
        idx++) {
        QSharedPointer<CDataWrapper> element(dataset_array->getCDataWrapperElementAtIndex(idx).release());
        if(element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE) &&
                element->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION)) {
          attribute_cache.insert(QString::fromStdString(element->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION)),
                                 element);
        }
    }
}

DatasetReader::~DatasetReader() {

}

unsigned int DatasetReader::getNumberOfParameter() {
    return attribute_cache.size();
}

QSharedPointer<DatasetAttributeReader> DatasetReader::getAttribute(const QString& attribute_name) {
    if(attribute_cache.contains(attribute_name)){
        return QSharedPointer<DatasetAttributeReader>(new DatasetAttributeReader(attribute_cache[(attribute_name)]));
    }
    return QSharedPointer<DatasetAttributeReader>();
}

QList<QString> DatasetReader::getAttributeNameList() {
    return attribute_cache.keys();
}

QList< QSharedPointer<DatasetAttributeReader> > DatasetReader::getAttributeList() {
    QList< QSharedPointer<DatasetAttributeReader> > result;
    foreach (QString k, attribute_cache.keys()) {
        result.push_back(getAttribute(k));
    }
    return result;
}
