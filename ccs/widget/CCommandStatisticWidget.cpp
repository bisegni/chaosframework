#include "CCommandStatisticWidget.h"
#include "ui_CCommandStatisticWidget.h"

using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::node_monitor;

CCommandStatisticWidget::CCommandStatisticWidget(QWidget *parent) :
    QWidget(parent),
    ChaosMonitorWidgetCompanion(chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit, this),
    ui(new Ui::CCommandStatisticWidget),
    data_found(false),
    online_status(chaos::metadata_service_client::node_monitor::OnlineStateUnknown){
    ui->setupUi(this);
    std::memset(&cmd_stat, 0, sizeof(BatchCommandStat));
}

CCommandStatisticWidget::~CCommandStatisticWidget() {
    deinitChaosContent();
    delete ui;
}


void CCommandStatisticWidget::initChaosContent() {
    trackNode();
}

void CCommandStatisticWidget::deinitChaosContent() {
    untrackNode();
}

void CCommandStatisticWidget::updateChaosContent() {}

void CCommandStatisticWidget::nodeChangedOnlineState(const std::string& node_uid,
                                                     OnlineState old_status,
                                                     OnlineState new_status) {
    online_status = new_status;
    QMetaObject::invokeMethod(this,
                              "updateUIState",
                              Qt::QueuedConnection);
}

void CCommandStatisticWidget::updatedDS(const std::string& control_unit_uid,
                                        int dataset_type,
                                        MapDatasetKeyValues& dataset_key_values) {
    data_found = true;
    if(dataset_type == DatasetTypeSystem) {
        if(dataset_key_values.count(chaos::DataPackSystemKey::DP_SYS_QUEUED_CMD)) {
            cmd_stat.queued_commands = dataset_key_values[chaos::DataPackSystemKey::DP_SYS_QUEUED_CMD].asUInt32();
        }

        if(dataset_key_values.count(chaos::DataPackSystemKey::DP_SYS_STACK_CMD)) {
            cmd_stat.stacked_commands = (uint32_t)dataset_key_values[chaos::DataPackSystemKey::DP_SYS_STACK_CMD].asUInt32();
        }
    }
    QMetaObject::invokeMethod(this,
                              "updateUIState",
                              Qt::QueuedConnection);
}

void CCommandStatisticWidget::noDSDataFound(const std::string& control_unit_uid,
                                            int dataset_type) {
    data_found = false;
    QMetaObject::invokeMethod(this,
                              "updateUIState",
                              Qt::QueuedConnection);
}

void CCommandStatisticWidget::updateUIState() {
    ui->labelQueuedCommandCurrentValue->setText(QString::number(cmd_stat.queued_commands));
    setStyleSheetColorForOnlineState(online_status,
                                     ui->labelQueuedCommandCurrentValue);

    ui->labelStackedCommandCurrentValue->setText(QString::number(cmd_stat.stacked_commands));
    setStyleSheetColorForOnlineState(online_status,
                                     ui->labelStackedCommandCurrentValue);
}

void CCommandStatisticWidget::on_pushButtonClearPendigCommandQueue_clicked() {
    submitApiResult("clear_pending_command",
                    GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::ClearCommandQueue)->execute(nodeUID().toStdString()));
}

void CCommandStatisticWidget::on_pushButtonKillRunningCommand_clicked() {
    submitApiResult("kill_running_command",
                    GET_CHAOS_API_PTR(chaos::metadata_service_client::api_proxy::node::KillCurrentCommand)->execute(nodeUID().toStdString()));


}
