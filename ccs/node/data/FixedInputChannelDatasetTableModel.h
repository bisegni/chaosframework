#ifndef FIXEDINPUTCHANNELDATASETTABLEMODEL_H
#define FIXEDINPUTCHANNELDATASETTABLEMODEL_H

#include "ChaosFixedCDataWrapperTableModel.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

#include <QVector>
#include <QSharedPointer>

class FixedInputChannelDatasetTableModel:
        public ChaosFixedCDataWrapperTableModel
{
    Q_OBJECT
public:
    FixedInputChannelDatasetTableModel(QObject *parent = 0);
    ~FixedInputChannelDatasetTableModel();
    void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset);
protected:
    QVector< QSharedPointer<chaos::common::data::CDataWrapper> > dataset_output_element;
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
};

#endif // FIXEDINPUTCHANNELDATASETTABLEMODEL_H
