#ifndef CHAOSSTORAGETYPEWIDGET_H
#define CHAOSSTORAGETYPEWIDGET_H

#include <QWidget>
#include "ChaosMonitorWidgetCompanion.h"
#include "CStateVisiblePushButton.h"

namespace Ui {
class ChaosStorageTypeWidget;
}

class ChaosStorageTypeWidget :
        public QWidget,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    Q_OBJECT
public:
    explicit ChaosStorageTypeWidget(QWidget *parent = 0);
    ~ChaosStorageTypeWidget();

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
    void updateUIStatus();
    void on_pushButton_clicked(bool clicked);

private:
    bool data_found;
    QString last_pushbutton_in_error;
    QString last_error_message;
    chaos::DataServiceNodeDefinitionType::DSStorageType storage_type;
    chaos::metadata_service_client::node_monitor::OnlineState online_status;
    Ui::ChaosStorageTypeWidget *ui;

    void sendStorageType(chaos::DataServiceNodeDefinitionType::DSStorageType type, const QString &event_tag);
    void apiHasStarted(const QString& api_tag);
    void apiHasEnded(const QString& api_tag);
    void apiHasEndedWithError(const QString& api_tag,
                              QSharedPointer<chaos::CException> api_exception);
};

#endif // CHAOSSTORAGETYPEWIDGET_H
