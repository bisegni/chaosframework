#ifndef CNODERESOURCEWIDGET_H
#define CNODERESOURCEWIDGET_H

#include "ChaosMonitorWidgetCompanion.h"

#include <QWidget>

namespace Ui {
class CNodeResourceWidget;
}

class CNodeResourceWidget :
        public QWidget,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::NodeMonitorHandler {
    Q_OBJECT

public:
    explicit CNodeResourceWidget(QWidget *parent = 0);
    ~CNodeResourceWidget();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();
protected:
    //! called when an online state has changed
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
protected slots:
    void updateUIStatus();
    void updateUIResource();
    void updateUIErrorInformation();
private:
    chaos::metadata_service_client::node_monitor::OnlineStatus current_status;
    chaos::metadata_service_client::node_monitor::ProcessResource current_resource;
    chaos::metadata_service_client::node_monitor::ErrorInformation current_error_information;
    Ui::CNodeResourceWidget *ui;
    QString secondsToDHMS(uint64_t duration);
};

#endif // CNODERESOURCEWIDGET_H
