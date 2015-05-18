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
    ui(new Ui::HealtPresenterWidget)
{
    ui->setupUi(this);
    ui->labelUID->setText(node_to_check);
    //ui->labelUID->setStyleSheet("QLabel { color : blue; }");

    QSharedPointer<QIcon> not_found_icon(new QIcon(":/images/white_circle_indicator.png"));
    QSharedPointer<QIcon> offline_icon(new QIcon(":/images/red_circle_indicator.png"));
    QSharedPointer<QIcon> on_icon(new QIcon(":/images/green_circle_indicator.png"));

    ui->ledIndicator->addState(0, not_found_icon);
    ui->ledIndicator->addState(1, offline_icon);
    ui->ledIndicator->addState(2, on_icon);

    //handler connection
    connect(&status_handler,
            SIGNAL(valueUpdated(QString,QVariant)),
            SLOT(updateAttributeValue(QString,QVariant)));
    connect(&hb_handler,
            SIGNAL(valueUpdated(QString,QVariant)),
            SLOT(updateAttributeValue(QString,QVariant)));
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
                                                                                 30,
                                                                                 &status_handler);
    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(node_key.toStdString(),
                                                                                 30,
                                                                                 &hb_handler);
    delete ui;
}

void HealtPresenterWidget::updateAttributeValue(const QString& attribute_name,
                                                const QVariant& attribute_value) {
    if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS) == 0) {
        //print the status
        ui->labelStatus->setText(attribute_value.toString());
    } else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP) == 0){
        //print the timestamp and update the red/green indicator
        if(attribute_value.isNull()) {
            ui->ledIndicator->setState(0);
        }else {
            quint64 time_diff = QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch() - attribute_value.toULongLong();
            //QDateTime::fromMSecsSinceEpoch(cur_ts, Qt::LocalTime).toString()
            if(time_diff <= 5000) {
                //in time
                ui->ledIndicator->setState(2);
            } else {
                //timeouted
                ui->ledIndicator->setState(1);
            }
        }
    }
}
