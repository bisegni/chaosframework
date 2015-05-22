#include "HealtPresenterWidget.h"
#include "ui_HealtPresenterWidget.h"

#include <QDateTime>
#include <QDebug>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::metadata_service_client;

HealtPresenterWidget::HealtPresenterWidget(const QString &node_to_check,
                                           QWidget *parent) :
    QFrame(parent),
    last_time_stamp(0),
    node_key(node_to_check),
    status_handler(true),
    ui(new Ui::HealtPresenterWidget)
{
    ui->setupUi(this);
    ui->labelUID->setText(node_to_check);

    //handler connection
    connect(&status_handler,
            SIGNAL(valueUpdated(QString,QString,QVariant)),
            SLOT(updateAttributeValue(QString,QString,QVariant)));
    connect(&hb_handler,
            SIGNAL(valueUpdated(QString,QString,QVariant)),
            SLOT(updateAttributeValue(QString,QString,QVariant)));
    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(node_key.toStdString(),
                                                                              20,
                                                                              &status_handler);
    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(node_key.toStdString(),
                                                                              20,
                                                                              &hb_handler);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(2);
    setMidLineWidth(2);
}

HealtPresenterWidget::~HealtPresenterWidget() {
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(node_key.toStdString(),
                                                                                 20,
                                                                                 &status_handler);
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(node_key.toStdString(),
                                                                                 20,
                                                                                 &hb_handler);
    delete ui;
}

void HealtPresenterWidget::updateAttributeValue(const QString& key,
                                                const QString& attribute_name,
                                                const QVariant& attribute_value) {
    if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS) == 0) {
        //print the status
        ui->labelStatus->setText(attribute_value.toString());
    } else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0){
        //print the timestamp and update the red/green indicator
        ui->ledIndicatorHelatTS->setNewTS(attribute_value.toULongLong());
    }
}
