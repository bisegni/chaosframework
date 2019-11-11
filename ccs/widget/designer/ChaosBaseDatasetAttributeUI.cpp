#include "ChaosBaseDatasetAttributeUI.h"

#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
ChaosBaseDatasetAttributeUI::ChaosBaseDatasetAttributeUI(QWidget *parent):
    ChaosBaseDatasetUI (parent){
    setAcceptDrops(true);
}

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
    if(attributeName().compare(attribute_name) != 0) return;
    qDebug() << "received value for: " << attribute_name<< "["<<attributeName()<<"] value:" << attribute_value.toString();
    ChaosBaseDatasetUI::updateData(dataset_type,
                                   attribute_name,
                                   attribute_value);
}

void ChaosBaseDatasetAttributeUI::changeSetCommitted() {}

void ChaosBaseDatasetAttributeUI::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/chaos-uid-dataset-attribute") ||
            event->mimeData()->hasFormat("application/chaos-node-uid-list")) {
        event->acceptProposedAction();
        chaosWidgetEditMode(true);
    } else {
        event->ignore();
    }
}

void ChaosBaseDatasetAttributeUI::dragMoveEvent(QDragMoveEvent* event) {
    Q_UNUSED(event)
}

void ChaosBaseDatasetAttributeUI::dragLeaveEvent(QDragLeaveEvent *event) {
    Q_UNUSED(event)
    chaosWidgetEditMode(false);
}

void ChaosBaseDatasetAttributeUI::dropEvent(QDropEvent *event) {
    chaosWidgetEditMode(false);
    if (event->mimeData()->hasFormat("application/chaos-uid-dataset-attribute")) {
        QByteArray itemData = event->mimeData()->data("application/chaos-uid-dataset-attribute");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString node_uid;
        int dataset;
        QString attribute_name;
        dataStream >> node_uid >> dataset >> attribute_name;
        setDeviceID(node_uid);
        setDatasetType(static_cast<DatasetType>(dataset));
        setAttributeName(attribute_name);
        qDebug() << QString("drop for %1-%2-%3").arg(node_uid).arg(dataset).arg(attribute_name);
        event->acceptProposedAction();
    } if(event->mimeData()->hasFormat("application/chaos-node-uid-list")) {
        QByteArray encoded = event->mimeData()->data("application/chaos-node-uid-list");
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        while (!stream.atEnd()) {
            QString node_uid;
            stream >> node_uid;
            setDeviceID(node_uid);
            qDebug() << QString("drop for %1").arg(node_uid);
        }
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

QJsonObject ChaosBaseDatasetAttributeUI::serialize() {
    QJsonObject result = ChaosBaseDatasetUI::serialize();
    if(attributeName().size()){result.insert("ChaosBaseDatasetAttributeUI::attributeName", attributeName());}
    return result;
}

void ChaosBaseDatasetAttributeUI::deserialize(QJsonObject& serialized_data) {
    ChaosBaseDatasetUI::deserialize(serialized_data);
    if(serialized_data.isEmpty()){
        return;
    }
    if(serialized_data.contains("ChaosBaseDatasetAttributeUI::attributeName")) {
        setAttributeName(serialized_data["ChaosBaseDatasetAttributeUI::attributeName"].toString());
    }
}
