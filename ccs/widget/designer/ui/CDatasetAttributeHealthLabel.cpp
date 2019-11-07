#include "CDatasetAttributeHealthLabel.h"
#include <chaos/common/chaos_constants.h>
#include <QHBoxLayout>

CDatasetAttributeHealthLabel::CDatasetAttributeHealthLabel(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent) {
    labelValue = new QLabel(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    labelValue->setText("Health Value");
    layout->addWidget(labelValue);
    layout->setSpacing(-1);
    layout->setMargin(0);
    setLayout(layout);
    setDatasetType(ChaosBaseDatasetUI::Health);
}

void CDatasetAttributeHealthLabel::setHealthValueType(const HealthValueType &new_health_type) {
    p_health_value_type = new_health_type;
    switch(p_health_value_type) {
    case HealthValueTypeTimeStamp: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP);break;
    case HealthValueTypeMDSTimeStamp: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP);break;
    case HealthValueTypeUserTime: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME); break;
    case HealthValueTypeSystemTime: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME); break;
    case HealthValueTypeProcessSwap: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP); break;
    case HealthValueTypeUptime: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME); break;
    case HealthValueTypeStatus: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS); break;
    case HealthValueTypeError:  setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_CODE); break;
    case HealthValueTypeErrorMessage: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_MESSAGE); break;
    case HealthValueTypeErrorDomain: setAttributeName(chaos::NodeHealtDefinitionKey::NODE_HEALT_LAST_ERROR_DOMAIN); break;
    }
}

CDatasetAttributeHealthLabel::HealthValueType CDatasetAttributeHealthLabel::healthValueType() const {return p_health_value_type;}

void CDatasetAttributeHealthLabel::updateOnline(ChaosBaseDatasetUI::OnlineState /*state*/) {}

void CDatasetAttributeHealthLabel::updateValue(QVariant new_value) {
    labelValue->setText(new_value.toString());
}
