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
                             chaos::metadata_service_client::node_monitor::OnlineStatus alive_state);

protected:
    void nodeChangedOnlineStatus(const std::string& node_uid,
                                 chaos::metadata_service_client::node_monitor::OnlineStatus old_status,
                                 chaos::metadata_service_client::node_monitor::OnlineStatus new_status);

    void nodeChangedProcessResource(const std::string& node_uid,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res);

    void nodeChangedErrorInformation(const std::string& node_uid,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& old_error_information,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation& new_error_information);

    void handlerHasBeenRegistered(const std::string& node_uid,
                                  const chaos::metadata_service_client::node_monitor::HealthInformation& current_health_status);

private slots:
    void updateStatusInfo();

private:
    chaos::metadata_service_client::node_monitor::OnlineStatus alive_state;
    QSharedPointer<QIcon> no_ts;
    QSharedPointer<QIcon> timeouted;
    QSharedPointer<QIcon> alive;
};

#endif // CLEDINDICATORHEALT_H
