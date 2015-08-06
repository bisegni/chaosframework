#ifndef CHAOSBYTEARRAY
#define CHAOSBYTEARRAY

#include <QObject>
#include <QByteArray>
#include <ChaosMetadataServiceClient/monitor_system/monitor_system.h>

class ChaosByteArray:
        public QObject {
    Q_OBJECT
public:
    ChaosByteArray(boost::shared_ptr<chaos::common::data::SerializationBuffer> _source_byte_array,
                   QObject *p = 0);

    QByteArray getByteArray();


private:
    boost::shared_ptr<chaos::common::data::SerializationBuffer> source_byte_array;
};
#endif // CHAOSBYTEARRAY

