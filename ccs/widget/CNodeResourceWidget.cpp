#include "CNodeResourceWidget.h"
#include "ui_CNodeResourceWidget.h"

using namespace chaos::metadata_service_client::monitor_system;

CNodeResourceWidget::CNodeResourceWidget(QWidget *parent) :
    QWidget(parent),
    ChaosMonitorWidgetCompanion(this),
    ui(new Ui::CNodeResourceWidget) {
    ui->setupUi(this);
    //set the target dataset for resource information
}

CNodeResourceWidget::~CNodeResourceWidget() {
    delete ui;
}

void CNodeResourceWidget::initChaosContent() {
    trackDataset();
}

void CNodeResourceWidget::deinitChaosContent() {
    untrackDataset();
}

void CNodeResourceWidget::updateChaosContent() {

}

void CNodeResourceWidget::updateUIStatus() {
    switch(current_status) {
    case chaos::metadata_service_client::node_monitor::OnlineStatusNotFound: {
        ui->labelUsrProc->setText(tr("---"));
        ui->labelSysProc->setText(tr("---"));
        ui->labelSwapProc->setText(tr("---"));
        ui->labelUptimeProc->setText(tr("---"));
        this->setEnabled(false);
        break;
    }
    case chaos::metadata_service_client::node_monitor::OnlineStatusUnknown:
    case chaos::metadata_service_client::node_monitor::OnlineStatusOFF:
    case chaos::metadata_service_client::node_monitor::OnlineStatusON:{
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
void CNodeResourceWidget::nodeChangedOnlineStatus(const std::string& node_uid,
                                                  chaos::metadata_service_client::node_monitor::OnlineStatus old_status,
                                                  chaos::metadata_service_client::node_monitor::OnlineStatus new_status) {
    current_status = new_status;
    QMetaObject::invokeMethod(this,
                              "updateUIStatus",
                              Qt::QueuedConnection);

}

void CNodeResourceWidget::nodeChangedProcessResource(const std::string& node_uid,
                                                     const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                                     const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res) {
    current_resource = new_proc_res;
//    QMetaObject::invokeMethod(this,
//                              "updateUIResources",
//                              Qt::QueuedConnection);
}

void CNodeResourceWidget::nodeChangedErrorInformation(const std::string& node_uid,
                                                      const chaos::metadata_service_client::node_monitor::ErrorInformation& old_error_information,
                                                      const chaos::metadata_service_client::node_monitor::ErrorInformation& new_error_information) {
    current_error_information = new_error_information;
    QMetaObject::invokeMethod(this,
                              "updateUIErrorInformation",
                              Qt::QueuedConnection);
}
