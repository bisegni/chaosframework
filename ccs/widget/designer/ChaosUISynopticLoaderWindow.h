#ifndef CHAOSUISYNOPTICLOADERWINDOW_H
#define CHAOSUISYNOPTICLOADERWINDOW_H

#include "CUNodeRoot.h"
#include "../ChaosMonitorWidgetCompanion.h"

#include <QJSEngine>
#include <QMap>
#include <QMainWindow>
#include <QCloseEvent>
#include <QSharedPointer>
#include <QUiLoader>

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

    //global changeset commit
    void commitChangeSet();
    //global change set rollback
    void rollbackChangeSet();

    void on_editWidgetAction_triggered();

    void on_saveUIFileAction_triggered();

private:
    bool ui_edit;
    bool ui_enabled;
    //contain last laoded ui in a comrpessed format
    QByteArray compressed_loaded_ui;
    QUiLoader formLoader;
    QSharedPointer<QJSEngine> js_engine;
    //contains correlation for cu and his associated ui;
    QMap<QString, QSharedPointer<CUNodeRoot> > map_device_root;
    Ui::ChaosUISynopticLoaderWindow *ui;

    QWidget *loadUiFile(QWidget *parent, QString filePath);
    // monitor handler
    void nodeChangedOnlineState(const std::string& control_unit_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                chaos::metadata_service_client::node_monitor::OnlineState new_status);

    void nodeChangedInternalState(const std::string& node_uid, const std::string&,
                                  const std::string& new_status);

    void nodeChangedProcessResource(const std::string& node_uid,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& old_proc_res,
                                    const chaos::metadata_service_client::node_monitor::ProcessResource& new_proc_res);

    void nodeChangedErrorInformation(const std::string& node_uid,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation&old_status,
                                     const chaos::metadata_service_client::node_monitor::ErrorInformation&new_status);

    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);

    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type);

    bool monitor(const QString& cu_uid, bool enable);

    QWidget *loadAsCUF(QString ui_file, QWidget *parent);

    QVariant toQVariant(chaos::common::data::CDataVariant chaos_value);
    void closeEvent(QCloseEvent *event);
    ChaosBaseDatasetUI *getChaosWidgetParent(QObject *w);
};

#endif // CHAOSUISYNOPTICLOADERWINDOW_H
