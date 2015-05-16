#include "HealtPresenterWidget.h"
#include "ui_HealtPresenterWidget.h"

#include <QDateTime>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::metadata_service_client;

void StatusHandler::consumeValue(const std::string& key,
                                 const std::string& attribute,
                                 const std::string& value){
    emit valueUpdated(QString::fromStdString(attribute),
                      QVariant::fromValue<QString>(QString::fromStdString(value)));
}

void HearbeatHandler::consumeValue(const std::string& key,
                                   const std::string& attribute,
                                   const int64_t value) {
    emit valueUpdated(QString::fromStdString(attribute),
                      QVariant::fromValue<qulonglong>(value));
}

HealtPresenterWidget::HealtPresenterWidget(const QString &node_to_check,
                                           QWidget *parent) :
    QFrame(parent),
    last_time_stamp(0),
    node_key(node_to_check),
    ui(new Ui::HealtPresenterWidget)
{
    ui->setupUi(this);
    ui->labelUID->setText(node_to_check);

    QSharedPointer<QIcon> offline_icon(new QIcon(":/images/red_circle_indicator.png"));
    QSharedPointer<QIcon> on_icon(new QIcon(":/images/green_circle_indicator.png"));

    ui->ledIndicator->addState(0, offline_icon);
    ui->ledIndicator->addState(1, on_icon);
//    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(node_key.toStdString(),
//                                                                              2,
//                                                                              &status_handler);
//    ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForHealt(node_key.toStdString(),
//                                                                              2,
//                                                                              &hb_handler);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(2);
    setMidLineWidth(2);
}

HealtPresenterWidget::~HealtPresenterWidget() {
//    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(node_key.toStdString(),
//                                                                                 2,
//                                                                                 &status_handler);
//    ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForHealt(node_key.toStdString(),
//                                                                                 2,
//                                                                                 &hb_handler);
    delete ui;
}

void HealtPresenterWidget::updateAttributeValue(const QString& attribute_name,
                                                const QVariant& attribute_value) {
    if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS)) {
        //print the status
        ui->labelStatus->setText(attribute_value.toString());
    } else if(attribute_name.compare(chaos::NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP)){
        //print the timestamp and update the red/green indicator

        qulonglong cur_ts = attribute_value.toULongLong();
        //QDateTime::fromMSecsSinceEpoch(cur_ts, Qt::LocalTime).toString()
        if(cur_ts > last_time_stamp) {
            //in time
            ui->ledIndicator->setState(1);
        } else if((QDateTime::currentMSecsSinceEpoch() - last_time_stamp) > 5000) {
            //timeouted
            ui->ledIndicator->setState(0);
        } else {
           ui->ledIndicator->setState(1);
        }
    }
}
