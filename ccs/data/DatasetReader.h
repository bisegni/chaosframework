#ifndef DATASETREADER_H
#define DATASETREADER_H

#include "DatasetAttributeReader.h"

#include <QMap>
#include <QObject>
#include <QSharedPointer>

class DatasetReader :
        public QObject {
    Q_OBJECT
public:
    explicit DatasetReader(QSharedPointer<chaos::common::data::CDataWrapper> _dataset_description,
                           QObject *parent = 0);
    ~DatasetReader();
    QSharedPointer<DatasetAttributeReader> getAttribute(const QString& attribute_name);
    QList<QString> getAttributeNameList();
    QList< QSharedPointer<DatasetAttributeReader> > getAttributeList();
    unsigned int getNumberOfParameter();
public slots:

private:
    QMap<QString, QSharedPointer<chaos::common::data::CDataWrapper> > attribute_cache;
    QSharedPointer<chaos::common::data::CDataWrapper> dataset_description;
};

#endif // DATASETREADER_H
