#include "CUNodeRoot.h"

CUNodeRoot::CUNodeRoot(QString device_id, QObject *parent) :
    QObject(parent),
m_device_id(device_id){}


void CUNodeRoot::setCurrentAttributeValue(QString attribute_name,
                              QVariant attribute_value) {
    emit updateDatasetAttribute(attribute_name,
                                attribute_value);
}
