#include "ChaosBaseDatasetAttributeUI.h"

#include <QMenu>
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
ChaosBaseDatasetAttributeUI::ChaosBaseDatasetAttributeUI(QWidget *parent):
    ChaosBaseDatasetUI (parent){
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::ActionsContextMenu);
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
    if (isInEditMode() &&
            (event->mimeData()->hasFormat("application/chaos-uid-dataset-attribute") ||
             event->mimeData()->hasFormat("application/chaos-node-uid-list"))) {
        event->accept();
        setStyleSheet("QFrame { border: 2px solid blue } ");
    } else {
        event->ignore();
    }
}

void ChaosBaseDatasetAttributeUI::dragMoveEvent(QDragMoveEvent* event) {
    Q_UNUSED(event)
}

void ChaosBaseDatasetAttributeUI::dragLeaveEvent(QDragLeaveEvent *event) {
    Q_UNUSED(event)
    setStyleSheet("QFrame { border: 2px solid red } ");
}

void ChaosBaseDatasetAttributeUI::dropEvent(QDropEvent *event) {
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
        updateEditInformation();
        event->accept();
    } if(event->mimeData()->hasFormat("application/chaos-node-uid-list")) {
        QByteArray encoded = event->mimeData()->data("application/chaos-node-uid-list");
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        while (!stream.atEnd()) {
            QString node_uid;
            stream >> node_uid;
            setDeviceID(node_uid);
            qDebug() << QString("drop for %1").arg(node_uid);
        }
        //trigger contextual menu for dataset type
        execContextualMenuForDatasetSelection(event->pos());
        updateEditInformation();
        event->accept();
    } else {
        event->ignore();
    }
    setStyleSheet("QFrame { border: 2px solid red } ");
}

void ChaosBaseDatasetAttributeUI::execContextualMenuForDatasetSelection(QPoint pos) {
    QMenu *contextMenu = new QMenu("DatasetContextualMenu");

    QAction *newAct = new QAction(tr("Output"), contextMenu);
    newAct->setData(Output);
    connect(newAct, &QAction::triggered, this, &ChaosBaseDatasetAttributeUI::datasetTriggerAction);
    contextMenu->addAction(newAct);

    newAct = new QAction(tr("Input"), contextMenu);
    newAct->setData(Input);
    connect(newAct, &QAction::triggered, this, &ChaosBaseDatasetAttributeUI::datasetTriggerAction);
    contextMenu->addAction(newAct);

    newAct = new QAction(tr("Custom"), contextMenu);
    newAct->setData(Custom);
    connect(newAct, &QAction::triggered, this, &ChaosBaseDatasetAttributeUI::datasetTriggerAction);
    contextMenu->addAction(newAct);

    newAct = new QAction(tr("System"), contextMenu);
    newAct->setData(Output);
    connect(newAct, &QAction::triggered, this, &ChaosBaseDatasetAttributeUI::datasetTriggerAction);
    contextMenu->addAction(newAct);

    newAct = new QAction(tr("Health"), contextMenu);
    newAct->setData(Input);
    connect(newAct, &QAction::triggered, this, &ChaosBaseDatasetAttributeUI::datasetTriggerAction);
    contextMenu->addAction(newAct);

    newAct = new QAction(tr("DevAlarm"), contextMenu);
    newAct->setData(Custom);
    connect(newAct, &QAction::triggered, this, &ChaosBaseDatasetAttributeUI::datasetTriggerAction);
    contextMenu->addAction(newAct);

    newAct = new QAction(tr("CUAlarm"), contextMenu);
    newAct->setData(Custom);
    connect(newAct, &QAction::triggered, this, &ChaosBaseDatasetAttributeUI::datasetTriggerAction);
    contextMenu->addAction(newAct);

    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    contextMenu->exec(mapToGlobal(pos));
}

void ChaosBaseDatasetAttributeUI::datasetTriggerAction(bool checked) {
    QAction* cm_action = qobject_cast<QAction*>(sender());
    setDatasetType(static_cast<DatasetType>(cm_action->data().toInt()));
    updateEditInformation();
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
    updateEditInformation();
}

void ChaosBaseDatasetAttributeUI::updateEditInformation() {
    QString richText = QString("<html><head/><body><p>Device: %1</p><p>Dataset: %2</p><p>Attribute: %3</p></body></html>").arg(deviceID()).arg(datasetType()).arg(attributeName());
    setToolTip(richText);
    updateValue(QString("%1[%2]-%3").arg(deviceID()).arg(datasetType()).arg(attributeName()));
}
