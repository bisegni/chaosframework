#include "CDatasetAttributeLabel.h"

#include <QDebug>
#include <QDateTime>
#include <QHBoxLayout>

#include <chaos/common/chaos_constants.h>

CDatasetAttributeLabel::CDatasetAttributeLabel(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent),
    p_attribute_type(Custom){
    labelValue = new QLabel(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(-1);
    layout->setMargin(0);
    labelValue->setText("Dataset Value");
    layout->addWidget(labelValue);
    setLayout(layout);
}

void CDatasetAttributeLabel::setAttributeType(const AttributeType &new_attribute_type) {
    p_attribute_type = new_attribute_type;
    switch(p_attribute_type) {
    case Timestamp: setAttributeName(chaos::DataPackCommonKey::DPCK_TIMESTAMP); break;
    case DeviceID: setAttributeName(chaos::DataPackCommonKey::DPCK_DEVICE_ID); break;
    case SequenceID: setAttributeName(chaos::DataPackCommonKey::DPCK_SEQ_ID); break;
    case RunID: setAttributeName(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID); break;
    default:
        break;
    }
}

CDatasetAttributeLabel::AttributeType CDatasetAttributeLabel::attributeType() const {
    return p_attribute_type;
}

void CDatasetAttributeLabel::updateOnline(ChaosBaseDatasetUI::OnlineState /*state*/) {}

void CDatasetAttributeLabel::updateValue(QVariant new_value) {
    QString value;
    if(attributeType() == Timestamp) {
        value = QDateTime::fromMSecsSinceEpoch(new_value.toLongLong(), Qt::LocalTime).toString();
    } else {
        value = new_value.toString();
    }
    labelValue->setText(value);
}
