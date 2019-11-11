#include "ChaosBaseUI.h"

ChaosBaseUI::ChaosBaseUI(QWidget *parent):
    QFrame(parent){}

QJsonObject ChaosBaseUI::serialize() {
    QJsonObject result;
    return result;
}
void ChaosBaseUI::deserialize(QJsonObject& serialized_data) {}
