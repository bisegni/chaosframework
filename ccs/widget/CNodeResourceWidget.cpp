#include "CNodeResourceWidget.h"
#include "ui_CNodeResourceWidget.h"

using namespace chaos::metadata_service_client::monitor_system;

CNodeResourceWidget::CNodeResourceWidget(QWidget *parent) :
    QWidget(parent),
    ChaosReadDatasetWidgetCompanion(),
    ui(new Ui::CNodeResourceWidget) {
    ui->setupUi(this);
    //set the target dataset for resource information
    ChaosReadDatasetWidgetCompanion::setDataset(ChaosReadDatasetWidgetCompanion::HelathDataset);
}

CNodeResourceWidget::~CNodeResourceWidget() {
    delete ui;
}

void CNodeResourceWidget::initChaosContent() {
    trackDataset(schedulerSlot());
}

void CNodeResourceWidget::deinitChaosContent() {
    untrackDataset(schedulerSlot());
}

void CNodeResourceWidget::updateChaosContent() {

}

CNodeResourceWidget::Dataset CNodeResourceWidget::dataset() {
    return ChaosReadDatasetWidgetCompanion::dataset();
}

void CNodeResourceWidget::setDataset(CNodeResourceWidget::Dataset dataset) {

}

void CNodeResourceWidget::quantumSlotHasData(const std::string& key,
                                             const chaos::metadata_service_client::monitor_system::KeyValue& value) {
    QMetaObject::invokeMethod(this,
                              "updateUIWithData",
                              Qt::QueuedConnection,
                              Q_ARG(chaos::metadata_service_client::monitor_system::KeyValue, value));
}

void CNodeResourceWidget::quantumSlotHasNoData(const std::string& key) {
    QMetaObject::invokeMethod(this,
                              "updateUIWithNoData",
                              Qt::QueuedConnection);
}

void CNodeResourceWidget::updateUIWithData(const chaos::metadata_service_client::monitor_system::KeyValue value) {
    if(value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME)){
        ui->labelUsrProc->setText(QString::number(value->getDoubleValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_USER_TIME), 'f', 1 ));
    } else {
        ui->labelUsrProc->setText("---");
    }
    if(value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME)){
        ui->labelSysProc->setText(QString::number(value->getDoubleValue(chaos::NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME), 'f', 1 ));
    } else {
        ui->labelSysProc->setText("---");
    }
    if(value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP)){
        ui->labelSwapProc->setText(QString::number(value->getInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP)));
    }else {
        ui->labelSwapProc->setText("---");
    }
    if(value->hasKey(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME)){
        ui->labelUptimeProc->setText(secondsToDHMS(value->getInt64Value(chaos::NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME)));
    }else {
        ui->labelUptimeProc->setText("---");
    }
}

void CNodeResourceWidget::updateUIWithNoData() {
    ui->labelUsrProc->setText(tr("---"));
    ui->labelSysProc->setText(tr("---"));
    ui->labelSwapProc->setText(tr("---"));
    ui->labelUptimeProc->setText(tr("---"));
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
