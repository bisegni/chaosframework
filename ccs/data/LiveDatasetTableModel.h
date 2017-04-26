#ifndef LIVEDATASETTABLEMODEL_H
#define LIVEDATASETTABLEMODEL_H

#include "ChaosAbstractTableModel.h"

#include <chaos/common/data/CDataVariant.h>

#include <QMap>

class LiveDatasetTableModel:
        public ChaosAbstractTableModel {
public:
    LiveDatasetTableModel(QObject *parent=0);
    void setDataset(const QString& _node_uid,
                    const QString& _dataset_key,
                    chaos::common::data::CDWShrdPtr live_dataset);
    void clear();
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    bool isCellEditable(const QModelIndex &index) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
    bool isCellSelectable(const QModelIndex &index) const;
    bool setCellData(const QModelIndex &index, const QVariant &value);
private:
    typedef QPair<QString, chaos::common::data::CDataVariant> DatasetElement;
    QString node_uid;
    QString dataset_key;
    QVector< DatasetElement > dataset_values;
};

#endif // LIVEDATASETTABLEMODEL_H
