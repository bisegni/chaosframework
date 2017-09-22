#ifndef CHAOSBYTEARRAY
#define CHAOSBYTEARRAY

#include <QObject>
#include <QByteArray>
#include <chaos_metadata_service_client/monitor_system/monitor_system.h>

class ChaosByteArray:
        public QObject {
    Q_OBJECT
public:
    ChaosByteArray(ChaosSharedPtr<chaos::common::data::SerializationBuffer> _source_byte_array,
                   QObject *p = 0);

    QByteArray getByteArray();


private:
    ChaosSharedPtr<chaos::common::data::SerializationBuffer> source_byte_array;
};
#endif // CHAOSBYTEARRAY

