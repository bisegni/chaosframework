#include "ChaosBaseDatasetUI.h"
#include <QVariant>
#include <QMetaMethod>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QFrame>

ChaosBaseDatasetUI::ChaosBaseDatasetUI(QWidget *parent):
    ChaosBaseUI(parent),
    edit_mode(false),
    p_dataset_type(Unset){}

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

QVariant ChaosBaseDatasetUI::value() {
    return p_value;
}

bool ChaosBaseDatasetUI::isInEditMode() {
    return edit_mode;
}

QJsonObject ChaosBaseDatasetUI::serialize() {
    QJsonObject result = ChaosBaseUI::serialize();
    if(deviceID().size()){result.insert("ChaosBaseDatasetUI::deviceID", deviceID());}
    if(datasetType()!=Unset)result.insert("ChaosBaseDatasetUI::datasetType", datasetType());
    return result;
}

void ChaosBaseDatasetUI::deserialize(QJsonObject& serialized_data) {
    if(serialized_data.isEmpty()){
        return;
    }
    if(serialized_data.contains("ChaosBaseDatasetUI::deviceID")) {
        setDeviceID(serialized_data["ChaosBaseDatasetUI::deviceID"].toString());
    }
    if(serialized_data.contains("ChaosBaseDatasetUI::datasetType")) {
        setDatasetType(static_cast<DatasetType>(serialized_data["ChaosBaseDatasetUI::datasetType"].toInt()));
    }
}

void ChaosBaseDatasetUI::updateOnlineStateSlot(int state) {
    setEnabled(state==OnlineStateON);
    updateOnline((OnlineState)state);
}

void ChaosBaseDatasetUI::updateData(int dataset_type,
                                    QString attribute_name,
                                    QVariant attribute_value) {
    if(datasetType() != dataset_type) return;
    //keep track of the new value
    updateValue(attribute_value);
    //emit the changed value
    emit attributeValueChanged(p_value, attribute_value);
    p_value = attribute_value;
}

void ChaosBaseDatasetUI::chaosWidgetEditMode(bool new_edit_mode) {
    edit_mode = new_edit_mode;
    if(edit_mode) {
        setStyleSheet("QFrame { border: 2px solid red } ");
    } else {
        setFrameStyle(QFrame::NoFrame);
        setStyleSheet("QFrame {} ");
    }
}
