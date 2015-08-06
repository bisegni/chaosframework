#ifndef DATASETATTRIBUTELISTMODEL_H
#define DATASETATTRIBUTELISTMODEL_H

#include "ChaosAbstractListModel.h"
#include "../../data/DatasetReader.h"
#include <QVector>
#include <QSharedPointer>

#include <chaos/common/data/CDataWrapper.h>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class DatasetAttributeListModel:
        public ChaosAbstractListModel {
    Q_OBJECT

public:
    explicit DatasetAttributeListModel(QObject *parent = 0);

    void updateDataset(const QSharedPointer<DatasetReader>& _dataset_reader);
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
private:
    QList<QString> attribute_name_list;
    QSharedPointer<DatasetReader> dataset_reader;
};
#endif // DATASETATTRIBUTELISTMODEL_H
