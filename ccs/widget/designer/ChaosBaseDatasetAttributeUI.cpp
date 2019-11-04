#include "ChaosBaseDatasetAttributeUI.h"

ChaosBaseDatasetAttributeUI::ChaosBaseDatasetAttributeUI(QWidget *parent):
    ChaosBaseDatasetUI (parent){}

QString ChaosBaseDatasetAttributeUI::attributeName() const {
    return p_attribute_name;
}

void ChaosBaseDatasetAttributeUI::setAttributeName(const QString& new_attribute_name) {
    p_attribute_name = new_attribute_name;
}


void ChaosBaseDatasetAttributeUI::updateData(QString attribute_name,
                                            QVariant attribute_value) {
    if(p_attribute_name.compare(attribute_name) != 0) return;
    ChaosBaseDatasetUI::updateData(attribute_name,
                                   attribute_value);
}
