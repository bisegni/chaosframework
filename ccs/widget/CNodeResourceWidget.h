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
    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                chaos::metadata_service_client::node_monitor::OnlineState new_state);

    void nodeChangedProcessResource(const std::string& node_uid,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res);

    void handlerHasBeenRegistered(const std::string& node_uid,
                                  const chaos::metadata_service_client::node_monitor::HealthInformation& current_health_state);
protected slots:
    void updateUIState();
    void updateUIResource();
    void updateUIErrorInformation();
private:
    chaos::metadata_service_client::node_monitor::OnlineState current_state;
    chaos::metadata_service_client::node_monitor::ProcessResource current_resource;
    chaos::metadata_service_client::node_monitor::ErrorInformation current_error_information;
    Ui::CNodeResourceWidget *ui;
    QString secondsToDHMS(uint64_t duration);
};

#endif // CNODERESOURCEWIDGET_H
