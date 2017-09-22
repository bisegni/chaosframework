#ifndef CHAOSABSTRACTDATASETTABLEMODEL_H
#define CHAOSABSTRACTDATASETTABLEMODEL_H

#include "ChaosAbstractTableModel.h"
#include "../monitor/handler/handler.h"

#include <QMap>

#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

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
typedef QMap< QString, QSharedPointer<AbstractAttributeHandler> > DoeMapHandler;
typedef QMap< QString, QSharedPointer<AbstractAttributeHandler> >::iterator DoeMapHandlerIterator;

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
                      const QVariant& value);
private:
    QString base64Encode(QString string);
};

#endif // CHAOSABSTRACTDATASETTABLEMODEL_H
