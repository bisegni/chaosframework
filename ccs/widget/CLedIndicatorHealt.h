#ifndef CLEDINDICATORHEALT_H
#define CLEDINDICATORHEALT_H

#include "LedIndicatorWidget.h"
#include "../monitor/handler/handler.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class CLedIndicatorHealt :
        public LedIndicatorWidget {
    Q_OBJECT

    QString p_node_uid;
    Q_PROPERTY(QString node_uid READ nodeUniqueID WRITE setNodeUniqueID)

public:
    explicit CLedIndicatorHealt(QWidget *parent = 0);
    ~CLedIndicatorHealt();
    void setNodeUniqueID(const QString& node_uid);
    QString nodeUniqueID();
    virtual int startMonitoring();
    virtual int stopMonitoring();
signals:
    void changedOnlineStatus(bool online);

public slots:

private slots:
    virtual void valueUpdated(const QString& node_uid,
                              const QString& attribute_name,
                              const QVariant& attribute_value);

private:
    HealthHartbeatHandler hb_health_handler;
private:
    bool last_online_state;
    QSharedPointer<QIcon> no_ts;
    QSharedPointer<QIcon> timeouted;
    QSharedPointer<QIcon> alive;
    uint64_t last_recevied_ts;

    void manageOnlineFlag(bool current_status);
};

#endif // CLEDINDICATORHEALT_H
