#include "ChaosByteArray.h"


ChaosByteArray::ChaosByteArray(boost::shared_ptr<chaos::common::data::SerializationBuffer> _source_byte_array,
                               QObject *p):
    QObject(p),
    source_byte_array(_source_byte_array){
}

QByteArray ChaosByteArray::getByteArray() {
    return QByteArray::fromRawData(source_byte_array->getBufferPtr(),
                                   source_byte_array->getBufferLen());
}
