#include "HealtPresenterWidget.h"
#include "ui_HealtPresenterWidget.h"

#include <QDateTime>
#include <QDebug>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::metadata_service_client;

HealtPresenterWidget::HealtPresenterWidget(const QString &node_to_check,
                                           QWidget *parent) :
    QFrame(parent),
    ui(new Ui::HealtPresenterWidget) {
    ui->setupUi(this);
    ui->labelUID->setText(node_to_check);

    ui->ledIndicatorHealt->setNodeUniqueID(node_to_check);
    ui->ledIndicatorHealt->startMonitoring();

    ui->labelStatus->setNodeUniqueID(node_to_check);
    ui->labelStatus->startMonitoring();

    //handler connection

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(2);
    setMidLineWidth(2);
}

HealtPresenterWidget::~HealtPresenterWidget() {
    ui->ledIndicatorHealt->stopMonitoring();
    ui->labelStatus->stopMonitoring();
    delete ui;
}
