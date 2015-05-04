#ifndef FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H
#define FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H

#include "ChaosAbstractNodeTableModel.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

#include <QVector>
#include <QSharedPointer>

class FixedOutputChannelDatasetTableModel:
        public ChaosAbstractNodeTableModel
{
    Q_OBJECT
public:
    FixedOutputChannelDatasetTableModel(const QVector< QSharedPointer<chaos::common::data::CDataWrapper> >& _dataset_output_element,
                                        QObject *parent = 0);
    ~FixedOutputChannelDatasetTableModel();
private:

    QVector< QSharedPointer<chaos::common::data::CDataWrapper> > dataset_output_element;
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;

};

#endif // FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H
