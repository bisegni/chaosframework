#ifndef CHAOSUISYNOPTICLOADERWINDOW_H
#define CHAOSUISYNOPTICLOADERWINDOW_H

#include "../ChaosMonitorWidgetCompanion.h"
#include "../../api_async_processor/ApiSubmitter.h"

#include <QMainWindow>
#include <QMultiHash>

namespace Ui {
class ChaosUISynopticLoaderWindow;
}

class ChaosUISynopticLoaderWindow :
        public QMainWindow,
        protected ApiHandler,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    Q_OBJECT
public:
    explicit ChaosUISynopticLoaderWindow(QWidget *parent = nullptr);
    ~ChaosUISynopticLoaderWindow();

private slots:
    void windowIsShown();

    void on_enableUIAction_triggered();

    void on_loadUIFileAction_triggered();

private:
    bool ui_enabled;
    //contains correlation for cu and his associated ui;
    QMultiHash<QString, QObject*> cu_object_hash;
    ApiSubmitter api_submitter;
    Ui::ChaosUISynopticLoaderWindow *ui;

    QWidget *loadUiFile(QWidget *parent, QString filePath);
    // monitor handler
    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_status,
                                chaos::metadata_service_client::node_monitor::OnlineState new_status);
    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);

    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);

    bool monitor(const QString& cu_uid, bool enable);

    // api submittion handler
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void onApiError(const QString& tag,
                    QSharedPointer<chaos::CException> api_exception);
    void onApiTimeout(const QString& tag) ;
    void apiHasStarted(const QString& api_tag);
    void apiHasEnded(const QString& api_tag);
    void apiHasEndedWithError(const QString& tag,
                              const QSharedPointer<chaos::CException> api_exception);
    QVariant toQVariant(chaos::common::data::CDataVariant chaos_value);
};

#endif // CHAOSUISYNOPTICLOADERWINDOW_H
