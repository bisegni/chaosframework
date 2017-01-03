#ifndef HEALTPRESENTERWIDGET_H
#define HEALTPRESENTERWIDGET_H
#include "../handler/healt/healt.h"
#include "../../api_async_processor/ApiSubmitter.h"
#include <QFrame>

namespace Ui {
class HealtPresenterWidget;
}

//! Widget that show the healt information about a node
class HealtPresenterWidget :
        public QFrame,
        protected ApiHandler {
    Q_OBJECT

protected:

    //!Api has ben called successfully
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

public:
    explicit HealtPresenterWidget(const QString& node_to_check,
                                  QWidget *parent = 0);
    ~HealtPresenterWidget();
public slots:
    //!open node editor
    void on_pushButtonOpenNodeEditor_clicked();

    void cuContextualmenuTrigger();
    private slots:
    void changedOnlineStatus(const QString& node_uid,
                             chaos::metadata_service_client::node_monitor::OnlineState online_state);
private:
    QString type;
    QString subtype;
    const QString node_uid;
    ApiSubmitter api_submitter;
    Ui::HealtPresenterWidget *ui;

    QString seconds_to_DHMS(uint64_t duration);
};

#endif // HEALTPRESENTERWIDGET_H
