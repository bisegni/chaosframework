#include "CNodeResourceWidget.h"
#include "ui_CNodeResourceWidget.h"
#include <QDebug>
using namespace chaos::metadata_service_client::monitor_system;

CNodeResourceWidget::CNodeResourceWidget(QWidget *parent) :
    QWidget(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNode, this),
    ui(new Ui::CNodeResourceWidget) {
    ui->setupUi(this);
    //set the target dataset for resource information
}

CNodeResourceWidget::~CNodeResourceWidget() {
    delete ui;
}

void CNodeResourceWidget::initChaosContent() {
    trackNode();
}

void CNodeResourceWidget::deinitChaosContent() {
    untrackNode();
}

void CNodeResourceWidget::updateChaosContent() {

}

void CNodeResourceWidget::updateUIState() {
    switch(current_state) {
    case chaos::metadata_service_client::node_monitor::OnlineStateNotFound: {
        ui->labelUsrProc->setText(tr("---"));
        ui->labelSysProc->setText(tr("---"));
        ui->labelSwapProc->setText(tr("---"));
        ui->labelUptimeProc->setText(tr("---"));
        this->setEnabled(false);
        break;
    }
    case chaos::metadata_service_client::node_monitor::OnlineStateUnknown:
    case chaos::metadata_service_client::node_monitor::OnlineStateOFF:
    case chaos::metadata_service_client::node_monitor::OnlineStateON:{
        this->setEnabled(true);
        break;
    }
    }
}

void CNodeResourceWidget::updateUIResource() {
    ui->labelUsrProc->setText(QString::number(current_resource.usr_res, 'f', 1 ));
    ui->labelSysProc->setText(QString::number(current_resource.sys_res, 'f', 1 ));
    ui->labelSwapProc->setText(QString::number(current_resource.swp_res));
    ui->labelUptimeProc->setText(secondsToDHMS(current_resource.uptime));
}

void CNodeResourceWidget::updateUIErrorInformation() {

}

QString CNodeResourceWidget::secondsToDHMS(uint64_t duration) {
    QString res;
    int seconds = (int) (duration % 60);
    duration /= 60;
    int minutes = (int) (duration % 60);
    duration /= 60;
    int hours = (int) (duration % 24);
    int days = (int) (duration / 24);
    if((hours == 0)&&(days == 0))
        return res.sprintf("00:00:%02d:%02d", minutes, seconds);
    if (days == 0)
        return res.sprintf("00:%02d:%02d:%02d", hours, minutes, seconds);
    return res.sprintf("%dd%02d:%02d:%02d", days, hours, minutes, seconds);
}

//! called when an online state has changed
void CNodeResourceWidget::nodeChangedOnlineState(const std::string& node_uid,
                                                  chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                                  chaos::metadata_service_client::node_monitor::OnlineState new_state) {
    current_state = new_state;
    QMetaObject::invokeMethod(this,
                              "updateUIState",
                              Qt::QueuedConnection);

}

void CNodeResourceWidget::nodeChangedProcessResource(const std::string& node_uid,
                                                     const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                                     const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res) {
    current_resource = new_proc_res;
    QMetaObject::invokeMethod(this,
                              "updateUIResource",
                              Qt::QueuedConnection);
}

void CNodeResourceWidget::handlerHasBeenRegistered(const std::string& node_uid,
                                                   const chaos::metadata_service_client::node_monitor::HealthInformation& current_health_state,
                                                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues &map_health_dataset) {
    current_state = current_health_state.online_state;
    QMetaObject::invokeMethod(this,
                              "updateUIState",
                              Qt::QueuedConnection);
    current_resource = current_health_state.process_resource;
    QMetaObject::invokeMethod(this,
                              "updateUIResource",
                              Qt::QueuedConnection);
}
