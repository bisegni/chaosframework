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

    Q_ENUMS(AliveState)
public:
    enum AliveState {
        Indeterminated,
        Offline,
        Online
    };

    explicit CLedIndicatorHealt(QWidget *parent = 0);
    ~CLedIndicatorHealt();
    void setNodeUniqueID(const QString& node_uid);
    QString nodeUniqueID();
    virtual int startMonitoring();
    virtual int stopMonitoring();
signals:
    void changedOnlineStatus(const QString& node_uid,
                             CLedIndicatorHealt::AliveState alive_state);

public slots:

private slots:
    virtual void valueUpdated(const QString& node_uid,
                              const QString& attribute_name,
                              const QVariant& attribute_value);
    virtual void valueNotFound(const QString& node_uid,
                              const QString& attribute_name);
private:
    HealthHartbeatHandler hb_health_handler;
    AliveState alive_state;
    QSharedPointer<QIcon> no_ts;
    QSharedPointer<QIcon> timeouted;
    QSharedPointer<QIcon> alive;
    uint64_t last_recevied_ts;

    void manageOnlineFlag(AliveState current_alive_state);
};

#endif // CLEDINDICATORHEALT_H
