#include "ChaosBaseDatasetUI.h"
#include <QVariant>
#include <QMetaMethod>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QFrame>

ChaosBaseDatasetUI::ChaosBaseDatasetUI(QWidget *parent):
    QFrame(parent),
    edit_mode(false){}

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

void ChaosBaseDatasetUI::updateOnlineStateSlot(int state) {
    setEnabled(state==OnlineStateON);
    updateOnline((OnlineState)state);
}

void ChaosBaseDatasetUI::updateData(int dataset_type,
                                    QString attribute_name,
                                    QVariant attribute_value) {
//    qDebug() << "ChaosBaseDatasetUI::updateData:" << dataset_type << "[" << datasetType() << "]" << " attrbiute:" << attribute_name;
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
//        setFrameStyle(QFrame::Panel | QFrame::Raised);
        setStyleSheet("QFrame { border: 2px solid red } ");
    } else {
        setFrameStyle(QFrame::NoFrame);
        setStyleSheet("QFrame {} ");
    }
}
