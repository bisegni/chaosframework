#ifndef ABSTRACTHEALTHANDLER
#define ABSTRACTHEALTHANDLER
#include <QObject>
#include <QVariant>
#include <QDebug>
#include <QString>
#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class AbstractAttributeHandler:
        public QObject {
    Q_OBJECT
signals:
    void valueUpdated(const QString& key,
                      const QString& attribute_name,
                      const QVariant& attribute_value);

    void valueNotFound(const QString& key,
                       const QString& attribute_name);
};
#endif // ABSTRACTHEALTHANDLER

