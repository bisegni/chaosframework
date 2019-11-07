#ifndef CHAOSUISYNOPTICLOADERWINDOW_H
#define CHAOSUISYNOPTICLOADERWINDOW_H

#include "CUNodeRoot.h"
#include "../ChaosMonitorWidgetCompanion.h"

#include <QJSEngine>
#include <QMultiHash>
#include <QMainWindow>
#include <QCloseEvent>
#include <QSharedPointer>

namespace Ui {
class ChaosUISynopticLoaderWindow;
}

class ChaosUISynopticLoaderWindow :
        public QMainWindow,
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler {
    Q_OBJECT
public:
    explicit ChaosUISynopticLoaderWindow(QWidget *parent = nullptr);
    ~ChaosUISynopticLoaderWindow();

private slots:
    void windowIsShown();

    void on_enableUIAction_triggered();

    void on_loadUIFileAction_triggered();

    void customMenuRequested(QPoint point);

    void editScript();
    void on_actionEdit_Script_triggered();

private:
    bool ui_enabled;
    QSharedPointer<QJSEngine> js_engine;
    //contains correlation for cu and his associated ui;
    QMultiHash<QString, QSharedPointer<CUNodeRoot> > hash_device_root;
    Ui::ChaosUISynopticLoaderWindow *ui;

    QWidget *loadUiFile(QWidget *parent, QString filePath);
    // monitor handler
    void nodeChangedOnlineState(const std::string& control_unit_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_status,
                                chaos::metadata_service_client::node_monitor::OnlineState new_status);

    void nodeChangedInternalState(const std::string& node_uid, const std::string&,
                                  const std::string& new_status);

    void nodeChangedProcessResource(const std::string& node_uid,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res);

    void nodeChangedErrorInformation(const std::string& node_uid,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation&,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation&);

    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);

    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);

    bool monitor(const QString& cu_uid, bool enable);

    QVariant toQVariant(chaos::common::data::CDataVariant chaos_value);
    void closeEvent(QCloseEvent *event);
    ChaosBaseDatasetUI *getChaosWidgetParent(QObject *w);
};

#endif // CHAOSUISYNOPTICLOADERWINDOW_H
