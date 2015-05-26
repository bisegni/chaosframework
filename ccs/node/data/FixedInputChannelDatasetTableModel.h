#ifndef FIXEDINPUTCHANNELDATASETTABLEMODEL_H
#define FIXEDINPUTCHANNELDATASETTABLEMODEL_H

#include "ChaosAbstractDataSetTableModel.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

#include <QVector>
#include <QSharedPointer>

class FixedInputChannelDatasetTableModel:
        public ChaosAbstractDataSetTableModel
{
    Q_OBJECT
public:
    FixedInputChannelDatasetTableModel(const QString& node_uid,
                                       unsigned int dataset_type,
                                       QObject *parent = 0);
    ~FixedInputChannelDatasetTableModel();
    void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
};

#endif // FIXEDINPUTCHANNELDATASETTABLEMODEL_H
