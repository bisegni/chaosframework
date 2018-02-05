#ifndef CCOMMANDSTATISTICWIDGET_H
#define CCOMMANDSTATISTICWIDGET_H

#include <QWidget>

#include <chaos/common/batch_command/BatchCommandSandboxEventHandler.h>

#include "ChaosMonitorWidgetCompanion.h"

namespace Ui {
class CCommandStatisticWidget;
}

class CCommandStatisticWidget :
        public QWidget,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    Q_OBJECT

public:
    explicit CCommandStatisticWidget(QWidget *parent = 0);
    ~CCommandStatisticWidget();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();
protected:
    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_status,
                                chaos::metadata_service_client::node_monitor::OnlineState new_status);

    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);

    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);
private slots:
    void updateUIState();

    void on_pushButtonClearPendigCommandQueue_clicked();

    void on_pushButtonKillRunningCommand_clicked();

private:
    Ui::CCommandStatisticWidget *ui;
    bool data_found;
    chaos::metadata_service_client::node_monitor::OnlineState online_status;
    chaos::common::batch_command::BatchCommandStat cmd_stat;
};

#endif // CCOMMANDSTATISTICWIDGET_H
