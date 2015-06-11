#ifndef CHAOSABSTRACTDATASETTABLEMODEL_H
#define CHAOSABSTRACTDATASETTABLEMODEL_H

#include "ChaosAbstractTableModel.h"
#include "../../monitor/healt/handler/handler.h"

#include <QMap>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

struct AttributeInfo {
    int row;
    int column;
    chaos::DataType::DataType type;

    AttributeInfo(int _row,
                  int _column,
                  chaos::DataType::DataType _type);
};

typedef QMap< QString, QSharedPointer<AttributeInfo> >           DoeMapAI;
typedef QMap< QString, QSharedPointer<AttributeInfo> >::iterator DoeMapAIIterator;
typedef QMap< QString, QSharedPointer<chaos::metadata_service_client::monitor_system::AbstractQuantumKeyAttributeHandler> > DoeMapHandler;
typedef QMap< QString, QSharedPointer<chaos::metadata_service_client::monitor_system::AbstractQuantumKeyAttributeHandler> >::iterator DoeMapHandlerIterator;

class ChaosAbstractDataSetTableModel:
        public ChaosAbstractTableModel {
    Q_OBJECT

public:
    ChaosAbstractDataSetTableModel(const QString& _node_uid,
                                   unsigned int _dataset_type,
                                   QObject *parent = 0);
    ~ChaosAbstractDataSetTableModel();
    //update the dataset
    virtual void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset) = 0;
    //! start or stop the monitoring of the dataset attribute
    void setAttributeMonitoring(bool enable);

protected:
    unsigned int                                                    quantum_multiplier;
    bool                                                            monitoring_enabled;
    QString                                                         node_uid;
    unsigned int                                                    dataset_type;
    DoeMapAI                                                        map_doe_attribute_name_index;
    QMap< int, QString >                                            map_doe_current_values;
    DoeMapHandler                                                   map_doe_handlers;
    QVector< QSharedPointer<chaos::common::data::CDataWrapper> >    vector_doe;
    QSharedPointer<chaos::common::data::CDataWrapper>               data_wrapped;

protected slots:
    void consumeValue(const QString& key,
                      const QString& attribute,
                      const bool value);
    void consumeValue(const QString& key,
                      const QString& attribute,
                      const int32_t value);
    void consumeValue(const QString& key,
                      const QString& attribute,
                      const int64_t value);
    void consumeValue(const QString& key,
                      const QString& attribute,
                      const double value);
    void consumeValue(const QString& key,
                      const QString& attribute,
                      const QString& value);
    void consumeValue(const QString& key,
                      const QString& attribute,
                      const boost::shared_ptr<chaos::common::data::SerializationBuffer>& value);
private:
    QString base64Encode(QString string);
};

#endif // CHAOSABSTRACTDATASETTABLEMODEL_H
