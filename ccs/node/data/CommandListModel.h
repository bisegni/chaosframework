#ifndef COMMANDLISTMODEL_H
#define COMMANDLISTMODEL_H

#include "ChaosAbstractListModel.h"

#include <QVector>
#include <QSharedPointer>

#include <chaos/common/data/CDataWrapper.h>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class CommandListModel:
        public ChaosAbstractListModel {
    Q_OBJECT

public:
    CommandListModel(QObject *parent = 0);

    void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset);
protected:
    int getRowCount() const;
    QString getHeaderForSection(int section) const;
    QVariant getRowData(int row) const;
private:
    QVector< QSharedPointer<chaos::common::data::CDataWrapper> > command_description_array;
};

#endif // COMMANDLISTMODEL_H
