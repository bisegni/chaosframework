#include "ChaosByteArray.h"


ChaosByteArray::ChaosByteArray(ChaosSharedPtr<chaos::common::data::SerializationBuffer> _source_byte_array,
                               QObject *p):
    QObject(p),
    source_byte_array(_source_byte_array){
}

QByteArray ChaosByteArray::getByteArray() {
    return QByteArray::fromRawData(source_byte_array->getBufferPtr(),
                                   source_byte_array->getBufferLen());
}
