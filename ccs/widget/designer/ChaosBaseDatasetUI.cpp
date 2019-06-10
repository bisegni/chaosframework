#include "ChaosBaseDatasetUI.h"

ChaosBaseDatasetUI::ChaosBaseDatasetUI(QWidget *parent):
    QWidget(parent) {

}

void ChaosBaseDatasetUI::setDeviceID(const QString &new_device_id) {
    my_device_id = new_device_id;
}

QString ChaosBaseDatasetUI::deviceID() const {
    return my_device_id;
}

void ChaosBaseDatasetUI::setDatasetType(const DatasetType &new_dataset_type) {
    my_dataset_type = new_dataset_type;
}

ChaosBaseDatasetUI::DatasetType ChaosBaseDatasetUI::datasetType() const {
    return my_dataset_type;
}

QString ChaosBaseDatasetUI::attributeName() const {
    return my_attribute_name;
}

void ChaosBaseDatasetUI::setAttributeName(const QString& new_attribute_name) {
    my_attribute_name = new_attribute_name;
}

void ChaosBaseDatasetUI::updateData(QString attribute_name,
                                    QVariant attribute_value) {
    if(attribute_name.compare(my_attribute_name) != 0) return;
    QMetaObject::invokeMethod(this,
                              "updateValue",
                              Qt::QueuedConnection,
                              Q_ARG(QVariant, attribute_value));
}

