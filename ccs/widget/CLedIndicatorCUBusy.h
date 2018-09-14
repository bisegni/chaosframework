#ifndef CLEDINDICATORCUBUSY_H
#define CLEDINDICATORCUBUSY_H

#include "StateImageIndicatorWidget.h"
#include "../monitor/handler/handler.h"
#include "ChaosMonitorWidgetCompanion.h"

class CLedIndicatorCUBusy :
        public StateImageIndicatorWidget,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    Q_OBJECT
public:

    explicit CLedIndicatorCUBusy(QWidget *parent = 0);
    ~CLedIndicatorCUBusy();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();
signals:
    void changedOnlineStatus(const QString& node_uid,
                             chaos::metadata_service_client::node_monitor::OnlineState alive_state);

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
    void updateUIStatus();

private:
    bool busy;
    chaos::metadata_service_client::node_monitor::OnlineState alive_state;
    QSharedPointer<QIcon> no_busy_icon;
    QSharedPointer<QIcon> busy_icon;
};

#endif // CLEDINDICATORCUBUSY_H
