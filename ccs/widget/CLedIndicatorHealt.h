#ifndef CLEDINDICATORHEALT_H
#define CLEDINDICATORHEALT_H

#include "LedIndicatorWidget.h"
#include "../monitor/handler/handler.h"
#include "ChaosMonitorWidgetCompanion.h"

class CLedIndicatorHealt :
        public LedIndicatorWidget,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::NodeMonitorHandler {
    Q_OBJECT
public:

    explicit CLedIndicatorHealt(QWidget *parent = 0);
    ~CLedIndicatorHealt();
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

    void handlerHasBeenRegistered(const std::string& node_uid,
                                  const chaos::metadata_service_client::node_monitor::HealthInformation& current_health_status,
                                  chaos::metadata_service_client::node_monitor::MapDatasetKeyValues &map_health_dataset);

private slots:
    void updateUIStatus();

private:
    chaos::metadata_service_client::node_monitor::OnlineState alive_state;
    QSharedPointer<QIcon> no_ts;
    QSharedPointer<QIcon> timeouted;
    QSharedPointer<QIcon> alive;
};

#endif // CLEDINDICATORHEALT_H
