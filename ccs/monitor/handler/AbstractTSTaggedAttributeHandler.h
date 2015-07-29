#ifndef ABSTRACTTSTAGGEDATTRIBUTEHANDLER
#define ABSTRACTTSTAGGEDATTRIBUTEHANDLER
#include <QObject>
#include <QVariant>
#include <QDebug>
#include <QString>
#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class AbstractTSTaggedAttributeHandler:
        public QObject {
    Q_OBJECT

signals:
    void valueUpdated(const QString& key,
                      const QString& attribute_name,
                      const uint64_t timestamp,
                      const QVariant& attribute_value);
    void valueNotFound(const QString& key,
                       const QString& attribute_name);
};

#endif // ABSTRACTTSTAGGEDATTRIBUTEHANDLER

