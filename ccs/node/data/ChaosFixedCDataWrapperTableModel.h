#ifndef DATASETOUTPUTATTRIBUTETABLEMODE_H
#define DATASETOUTPUTATTRIBUTETABLEMODE_H

#include <QVector>

#include "ChaosAbstractTableModel.h"

class ChaosFixedCDataWrapperTableModel :
        public ChaosAbstractTableModel
{
public:
    ChaosFixedCDataWrapperTableModel(QObject *parent = 0);
    void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _data_wrapped);
protected:
    QSharedPointer<chaos::common::data::CDataWrapper> data_wrapped;
};

#endif // DATASETOUTPUTATTRIBUTETABLEMODE_H
