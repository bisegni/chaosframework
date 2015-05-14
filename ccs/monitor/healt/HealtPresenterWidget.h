#ifndef HEALTPRESENTERWIDGET_H
#define HEALTPRESENTERWIDGET_H

#include <QWidget>

#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

namespace Ui {
class HealtPresenterWidget;
}

class StatusHandler:
        public QObject,
        public chaos::metadata_service_client::monitor_system::handler::HealtStatusAttributeHandler
{
    Q_OBJECT

protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const std::string& value);
signals:
    void valueUpdated(const QString& attribute_name,
                      const QVariant& attribute_value);
};

class HearbeatHandler:
        public QObject,
        public chaos::metadata_service_client::monitor_system::handler::HealtHeartBeatAttributeHandler
{
    Q_OBJECT
protected:
    void consumeValue(const std::string& key,
                      const std::string& attribute,
                      const int64_t value);

signals:
    void valueUpdated(const QString& attribute_name,
                      const QVariant& attribute_value);
};


//! Widget that show the healt information about a node
class HealtPresenterWidget :
        public QWidget
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
    StatusHandler   status_handler;
    HearbeatHandler hb_handler;
    QString         node_key;
    qulonglong last_time_stamp;
    Ui::HealtPresenterWidget *ui;
};

#endif // HEALTPRESENTERWIDGET_H
