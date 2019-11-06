#include "ChaosBaseDatasetAttributeUI.h"
#include <QDebug>
ChaosBaseDatasetAttributeUI::ChaosBaseDatasetAttributeUI(QWidget *parent):
    ChaosBaseDatasetUI (parent){}

QString ChaosBaseDatasetAttributeUI::attributeName() const {
    return p_attribute_name;
}

void ChaosBaseDatasetAttributeUI::setAttributeName(const QString& new_attribute_name) {
    p_attribute_name = new_attribute_name;
}


void ChaosBaseDatasetAttributeUI::updateData(int dataset_type,
                                             QString attribute_name,
                                             QVariant attribute_value) {
    //check for right data to manage
    qDebug() << "received value for: " << attribute_name << " we are:" << attributeName();
    if(attributeName().compare(attribute_name) != 0) return;
    ChaosBaseDatasetUI::updateData(dataset_type,
                                   attribute_name,
                                   attribute_value);
}
