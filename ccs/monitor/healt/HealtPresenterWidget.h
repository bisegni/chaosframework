#ifndef HEALTPRESENTERWIDGET_H
#define HEALTPRESENTERWIDGET_H

#include "handler/handler.h"

#include <QFrame>

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

namespace Ui {
class HealtPresenterWidget;
}

//! Widget that show the healt information about a node
class HealtPresenterWidget :
        public QFrame
{
    Q_OBJECT

protected:
    //! called when there is a new value for the status


    //! called when there is a new value for the heartbeat

public:
    explicit HealtPresenterWidget(const QString& node_to_check,
                                  QWidget *parent = 0);
    ~HealtPresenterWidget();
public slots:
    void updateAttributeValue(const QString& attribute_name,
                              const QVariant& attribute_value);
private:
    HealtStatusHandler      status_handler;
    HealthHartbeatHandler   hb_handler;
    QString         node_key;
    qulonglong last_time_stamp;
    Ui::HealtPresenterWidget *ui;
};

#endif // HEALTPRESENTERWIDGET_H
