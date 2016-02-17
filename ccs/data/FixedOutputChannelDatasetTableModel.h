#ifndef FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H
#define FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H

#include "ChaosAbstractDataSetTableModel.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

#include <QVector>
#include <QSharedPointer>

class FixedOutputChannelDatasetTableModel:
        public ChaosAbstractDataSetTableModel
{
    Q_OBJECT
public:
    FixedOutputChannelDatasetTableModel(const QString& node_uid,
                                        unsigned int dataset_type,
                                        QObject *parent = 0);
    ~FixedOutputChannelDatasetTableModel();

    void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
private:
    QString getSubTypeForCode(int code) const;
};

#endif // FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H
