#ifndef CPROPERTYTEXTEDIT_H
#define CPROPERTYTEXTEDIT_H

#include <QWidget>
#include "ChaosMonitorWidgetCompanion.h"
namespace Ui {
class CPropertyTextEdit;
}

class CPropertyTextEdit :
        public QWidget,
        public ChaosMonitorWidgetCompanion,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    Q_OBJECT

public:
    explicit CPropertyTextEdit(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    ~CPropertyTextEdit();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();
    void setPropertyGroup(const QString& new_property_group);
    const QString& propertyGroup();

    void setPropertyName(const QString& new_property_name);
    const QString& propertyName();
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
    void on_pushButtonSavePropertyValue_clicked();

private:
    void apiHasStarted(const QString& api_tag);
    void apiHasEnded(const QString& api_tag);
    void apiHasEndedWithError(const QString& api_tag,
                              QSharedPointer<chaos::CException> api_exception);
    QString current_property_value;
    QString last_error_message;
    QString property_group;
    QString property_name;
    chaos::metadata_service_client::node_monitor::OnlineState online_status;
    Ui::CPropertyTextEdit *ui;
};

#endif // CPROPERTYTEXTEDIT_H
