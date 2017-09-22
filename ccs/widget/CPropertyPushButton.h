#ifndef CPROPERTYPUSHBUTTON_H
#define CPROPERTYPUSHBUTTON_H

#include <widget/CStateVisiblePushButton.h>
#include <widget/ChaosMonitorWidgetCompanion.h>

class CPropertyPushButton:
        public CStateVisiblePushButton,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    Q_OBJECT
public:
    CPropertyPushButton(QWidget *parent = 0);
    ~CPropertyPushButton();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();
    void setPropertyGroupName(QString _property_group_name);
    void setPropertyName(QString _property_name);
protected:
    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_status,
                                chaos::metadata_service_client::node_monitor::OnlineState new_status);
    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);

    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);
    void apiHasEndedWithError(const QString& api_tag,
                              QSharedPointer<chaos::CException> api_exception);
private slots:
    void updateUIStatus();
    void on_pushButton_clicked(bool clicked);
private:
    bool data_found;
    QString property_name;
    QString property_group_name;

    int last_error_code;
    QString last_error_message;
    chaos::common::data::CDataVariant last_got_value;
    chaos::metadata_service_client::node_monitor::OnlineState online_status;
};

#endif // CPROPERTYPUSHBUTTON_H
