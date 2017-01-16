#ifndef HEALTPRESENTERWIDGET_H
#define HEALTPRESENTERWIDGET_H
#include "../handler/healt/healt.h"
#include "../../api_async_processor/ApiSubmitter.h"
#include "utility/WidgetUtility.h"
#include <QFrame>

namespace Ui {
class HealtPresenterWidget;
}

//! Widget that show the healt information about a node
class HealtPresenterWidget :
        public QFrame,
        protected WidgetUtilityhandler,
        protected ApiHandler,
        chaos::metadata_service_client::node_monitor::NodeMonitorHandler {
    Q_OBJECT

protected:

    //!Api has ben called successfully
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void nodeChangedOnlineState(const std::string& node_uid,
                                chaos::metadata_service_client::node_monitor::OnlineState old_state,
                                chaos::metadata_service_client::node_monitor::OnlineState new_state);

    void nodeChangedInternalState(const std::string& node_uid,
                                  const std::string& old_state,
                                  const std::string& new_state);

    void nodeHasBeenRestarted(const std::string& node_uid);

    void cmActionTrigger(const QString& cm_title,
                         const QVariant& cm_data);
public:
    explicit HealtPresenterWidget(const QString& node_to_check,
                                  QWidget *parent = 0);
    ~HealtPresenterWidget();
public slots:
    //!open node editor
    void on_pushButtonOpenNodeEditor_clicked();
private slots:
    void changedOnlineStatus(const QString& node_uid,
                             chaos::metadata_service_client::node_monitor::OnlineState online_state);
private:
    bool is_cu;
    bool is_sc_cu;
    bool is_ds;
    QAction *node_action;
    QString type;
    QString subtype;
    WidgetUtility wUtil;
    const QString node_uid;
    ApiSubmitter api_submitter;
    Ui::HealtPresenterWidget *ui;

    QString seconds_to_DHMS(uint64_t duration);
};

#endif // HEALTPRESENTERWIDGET_H
