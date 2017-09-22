#include "ChaosAbstractDataSetTableModel.h"
#include "../monitor/handler/healt/healt.h"

#include <boost/lexical_cast.hpp>
#include <QDebug>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::monitor_system;

AttributeInfo::AttributeInfo(int _row,
                             int _column,
                             chaos::DataType::DataType _type):
    row(_row),
    column(_column),
    type(_type){}

ChaosAbstractDataSetTableModel::ChaosAbstractDataSetTableModel(const QString &_node_uid,
                                                               unsigned int _dataset_type,
                                                               QObject *parent):
    ChaosAbstractTableModel(parent),
    monitoring_enabled(false),
    node_uid(_node_uid),
    dataset_type(_dataset_type),
    quantum_multiplier(10){}

ChaosAbstractDataSetTableModel::~ChaosAbstractDataSetTableModel() {
}

void ChaosAbstractDataSetTableModel::setAttributeMonitoring(bool enable) {
    if(enable == monitoring_enabled) return;

    beginResetModel();
    monitoring_enabled = enable;
    endResetModel();

    QSharedPointer<AbstractAttributeHandler> attribute_handler;
    for(DoeMapAIIterator it = map_doe_attribute_name_index.begin();
        it != map_doe_attribute_name_index.end();
        it++) {

        if(enable) {
            qDebug()<< "Starting monitor for attribute:" << it.key();
            switch(it.value()->type) {
            case chaos::DataType::TYPE_BOOLEAN:{
                attribute_handler = QSharedPointer<AbstractAttributeHandler>(new MonitorBoolAttributeHandler(it.key()));
                break;
            }
            case chaos::DataType::TYPE_INT32:{
                attribute_handler = QSharedPointer<AbstractAttributeHandler>(new MonitorInt32AttributeHandler(it.key()));
                break;
            }
            case chaos::DataType::TYPE_INT64:{
                attribute_handler = QSharedPointer<AbstractAttributeHandler>(new MonitorInt64AttributeHandler(it.key()));
                break;
            }
            case chaos::DataType::TYPE_DOUBLE:{
                attribute_handler = QSharedPointer<AbstractAttributeHandler>(new MonitorDoubleAttributeHandler(it.key()));
                break;
            }
                case chaos::DataType::TYPE_CLUSTER:
            case chaos::DataType::TYPE_STRING:{
                attribute_handler = QSharedPointer<AbstractAttributeHandler>(new MonitorStringAttributeHandler(it.key()));
                break;
            }
            case chaos::DataType::TYPE_BYTEARRAY:{
                attribute_handler = QSharedPointer<AbstractAttributeHandler>(new MonitorBinaryAttributeHandler(it.key()));
                break;
            }
            }

            if(!attribute_handler.isNull()) {
                connect(attribute_handler.data(),
                        SIGNAL(valueUpdated(QString,QString,QVariant)),
                        SLOT(consumeValue(QString,QString,QVariant)));

                map_doe_handlers.insert(it.key(), attribute_handler);
                ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForDataset(node_uid.toStdString(),
                                                                                            dataset_type,
                                                                                            quantum_multiplier,
                                                                                            attribute_handler->getQuantumAttributeHandler());
            }
        } else {
            DoeMapHandlerIterator h_it = map_doe_handlers.find(it.key());
            if(h_it==map_doe_handlers.end()) continue;
            qDebug()<< "Stopping monitor for attribute:" << it.key();
            ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForDataset(node_uid.toStdString(),
                                                                                           dataset_type,
                                                                                           quantum_multiplier,
                                                                                           h_it.value()->getQuantumAttributeHandler());
            map_doe_handlers.erase(h_it);
        }
    }
}

void ChaosAbstractDataSetTableModel::consumeValue(const QString& key,
                                                  const QString& attribute,
                                                  const QVariant& value) {
    DoeMapAIIterator it = map_doe_attribute_name_index.find(attribute);
    if(it == map_doe_attribute_name_index.end()) return;
    if(!value.isValid()) return;

    switch(it.value()->type) {
    case chaos::DataType::TYPE_BOOLEAN:
    case chaos::DataType::TYPE_INT32:
    case chaos::DataType::TYPE_INT64:
    case chaos::DataType::TYPE_DOUBLE:
        case chaos::DataType::TYPE_CLUSTER:
    case chaos::DataType::TYPE_STRING:{
        map_doe_current_values.insert(it.value()->row,
                                      value.toString());
        break;
    }
    case chaos::DataType::TYPE_BYTEARRAY:{
        QSharedPointer<ChaosByteArray> value_buffer = value.value< QSharedPointer<ChaosByteArray> >();
        if(!value_buffer.isNull()){
            map_doe_current_values.insert(it.value()->row,
                                          base64Encode(QString(QByteArray::fromRawData(value_buffer->getByteArray().data(),
                                                                                       (value_buffer->getByteArray().count()<20?value_buffer->getByteArray().count():20)))));
        }
        break;
    }
    }

    QModelIndex index_to_refresh = this->index(it.value()->row, it.value()->column);
    emit(dataChanged(index_to_refresh, index_to_refresh));
}

QString ChaosAbstractDataSetTableModel::base64Encode(QString string) {
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}
