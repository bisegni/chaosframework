#include "ChaosBaseDatasetUI.h"
#include <QVariant>
#include <QMetaMethod>
#include <QMenu>
#include <QAction>
#include <QDebug>

ChaosBaseDatasetUI::ChaosBaseDatasetUI(QWidget *parent):
    QWidget(parent) {}

void ChaosBaseDatasetUI::setDeviceID(const QString &new_device_id) {
    p_device_id = new_device_id;
}

QString ChaosBaseDatasetUI::deviceID() const {
    return p_device_id;
}

void ChaosBaseDatasetUI::setDatasetType(const DatasetType &new_dataset_type) {
    p_dataset_type = new_dataset_type;
}

ChaosBaseDatasetUI::DatasetType ChaosBaseDatasetUI::datasetType() const {
    return p_dataset_type;
}

QString ChaosBaseDatasetUI::attributeName() const {
    return p_attribute_name;
}

void ChaosBaseDatasetUI::setAttributeName(const QString& new_attribute_name) {
    p_attribute_name = new_attribute_name;
}

void ChaosBaseDatasetUI::updateData(QString attribute_name,
                                    QVariant attribute_value) {
    if(attribute_name.compare(p_attribute_name) != 0) return;
    QMetaObject::invokeMethod(this,
                              "updateValue",
                              Qt::QueuedConnection,
                              Q_ARG(QVariant, attribute_value));
    //emit the changed value
    emit attributeValueChanged(p_value, attribute_value);
    //keep track of the new value
    p_value = attribute_value;
}
