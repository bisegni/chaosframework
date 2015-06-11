#include "ChaosAbstractDataSetTableModel.h"
#include "../../monitor/healt/handler/handler.h"

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
    quantum_multiplier(20){}

ChaosAbstractDataSetTableModel::~ChaosAbstractDataSetTableModel() {
}

void ChaosAbstractDataSetTableModel::setAttributeMonitoring(bool enable) {
    if(enable == monitoring_enabled) return;

    beginResetModel();
    monitoring_enabled = enable;
    endResetModel();

    QSharedPointer<AbstractQuantumKeyAttributeHandler> handler;
    for(DoeMapAIIterator it = map_doe_attribute_name_index.begin();
        it != map_doe_attribute_name_index.end();
        it++) {

        if(enable) {
            qDebug()<< "Starting monitor for attribute:" << it.key();
            switch(it.value()->type) {
            case chaos::DataType::TYPE_BOOLEAN:{
                MonitorBoolAttributeHandler *m_handler = new MonitorBoolAttributeHandler(it.key());
                if(!m_handler) break;
                handler = QSharedPointer<AbstractQuantumKeyAttributeHandler>(m_handler);
                connect(m_handler,
                        SIGNAL(valueUpdated(QString,QString,bool)),
                        SLOT(consumeValue(QString,QString,bool)));
                break;
            }
            case chaos::DataType::TYPE_INT32:{
                MonitorInt32AttributeHandler *m_handler = new MonitorInt32AttributeHandler(it.key());
                if(!m_handler) break;
                handler = QSharedPointer<AbstractQuantumKeyAttributeHandler>(m_handler);
                connect(m_handler,
                        SIGNAL(valueUpdated(QString,QString,int32_t)),
                        SLOT(consumeValue(QString,QString,int32_t)));
                break;
            }
            case chaos::DataType::TYPE_INT64:{
                MonitorInt64AttributeHandler *m_handler = new MonitorInt64AttributeHandler(it.key());
                if(!m_handler) break;
                handler = QSharedPointer<AbstractQuantumKeyAttributeHandler>(m_handler);
                connect(m_handler,
                        SIGNAL(valueUpdated(QString,QString,int64_t)),
                        SLOT(consumeValue(QString,QString,int64_t)));
                break;
            }
            case chaos::DataType::TYPE_DOUBLE:{
                MonitorDoubleAttributeHandler *m_handler = new MonitorDoubleAttributeHandler(it.key());
                if(!m_handler) break;
                handler = QSharedPointer<AbstractQuantumKeyAttributeHandler>(m_handler);
                connect(m_handler,
                        SIGNAL(valueUpdated(QString,QString,double)),
                        SLOT(consumeValue(QString,QString,double)));
                break;
            }
            case chaos::DataType::TYPE_STRING:{
                MonitorStringAttributeHandler *m_handler = new MonitorStringAttributeHandler(it.key());
                if(!m_handler) break;
                handler = QSharedPointer<AbstractQuantumKeyAttributeHandler>(m_handler);
                connect(m_handler,
                        SIGNAL(valueUpdated(QString,QString,QString)),
                        SLOT(consumeValue(QString,QString,QString)));
                break;
            }
            case chaos::DataType::TYPE_BYTEARRAY:{
                MonitorBinaryAttributeHandler *m_handler = new MonitorBinaryAttributeHandler(it.key());
                if(!m_handler) break;
                handler = QSharedPointer<AbstractQuantumKeyAttributeHandler>(m_handler);
                connect(m_handler,
                        SIGNAL(valueUpdated(QString,QString,boost::shared_ptr<chaos::common::data::SerializationBuffer>)),
                        SLOT(consumeValue(QString,QString,boost::shared_ptr<chaos::common::data::SerializationBuffer>)));
                break;
            }
            }

            if(handler) {
                map_doe_handlers.insert(it.key(), handler);
                ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForDataset(node_uid.toStdString(),
                                                                                            dataset_type,
                                                                                            quantum_multiplier,
                                                                                            handler.data());
            }
        } else {
            DoeMapHandlerIterator h_it = map_doe_handlers.find(it.key());
            if(h_it==map_doe_handlers.end()) continue;
            qDebug()<< "Stopping monitor for attribute:" << it.key();
            ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForDataset(node_uid.toStdString(),
                                                                                           dataset_type,
                                                                                           quantum_multiplier,
                                                                                           h_it.value().data());
            map_doe_handlers.erase(h_it);
        }
    }
}

void ChaosAbstractDataSetTableModel::consumeValue(const QString& key,
                                                  const QString& attribute,
                                                  const bool value) {
    DoeMapAIIterator it = map_doe_attribute_name_index.find(attribute);
    if(it == map_doe_attribute_name_index.end()) return;
    map_doe_current_values.insert(it.value()->row,
                                  QString::number(value));
    QModelIndex index_to_refresh = this->index(it.value()->row, it.value()->column);
    emit(dataChanged(index_to_refresh, index_to_refresh));
}

void ChaosAbstractDataSetTableModel::consumeValue(const QString& key,
                                                  const QString& attribute,
                                                  const int32_t value) {
    DoeMapAIIterator it = map_doe_attribute_name_index.find(attribute);
    if(it == map_doe_attribute_name_index.end()) return;
    map_doe_current_values.insert(it.value()->row,
                                  QString::number(value));
    QModelIndex index_to_refresh = this->index(it.value()->row, it.value()->column);
    emit(dataChanged(index_to_refresh, index_to_refresh));
}

void ChaosAbstractDataSetTableModel::consumeValue(const QString& key,
                                                  const QString& attribute,
                                                  const int64_t value) {
    DoeMapAIIterator it = map_doe_attribute_name_index.find(attribute);
    if(it == map_doe_attribute_name_index.end()) return;
    map_doe_current_values.insert(it.value()->row,
                                  QString::number(value));
    QModelIndex index_to_refresh = this->index(it.value()->row, it.value()->column);
    emit(dataChanged(index_to_refresh, index_to_refresh));
}

void ChaosAbstractDataSetTableModel::consumeValue(const QString& key,
                                                  const QString& attribute,
                                                  const double value) {
    DoeMapAIIterator it = map_doe_attribute_name_index.find(attribute);
    if(it == map_doe_attribute_name_index.end()) return;
    map_doe_current_values.insert(it.value()->row,
                                  QString::number(value));
    QModelIndex index_to_refresh = this->index(it.value()->row, it.value()->column);
    emit(dataChanged(index_to_refresh, index_to_refresh));
}

void ChaosAbstractDataSetTableModel::consumeValue(const QString& key,
                                                  const QString& attribute,
                                                  const QString& value) {
    DoeMapAIIterator it = map_doe_attribute_name_index.find(attribute);
    if(it == map_doe_attribute_name_index.end()) return;
    map_doe_current_values.insert(it.value()->row,
                                  value);
    QModelIndex index_to_refresh = this->index(it.value()->row, it.value()->column);
    emit(dataChanged(index_to_refresh, index_to_refresh));
}

void ChaosAbstractDataSetTableModel::consumeValue(const QString& key,
                                                  const QString& attribute,
                                                  const boost::shared_ptr<chaos::common::data::SerializationBuffer>& value) {
    DoeMapAIIterator it = map_doe_attribute_name_index.find(attribute);
    if(it == map_doe_attribute_name_index.end()) return;
    map_doe_current_values.insert(it.value()->row,
                                  base64Encode(QString(QByteArray::fromRawData(value->getBufferPtr(),
                                                                               (value->getBufferLen()<20?value->getBufferLen():20)))));
    QModelIndex index_to_refresh = this->index(it.value()->row, it.value()->column);
    emit(dataChanged(index_to_refresh, index_to_refresh));
}

QString ChaosAbstractDataSetTableModel::base64Encode(QString string) {
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}
