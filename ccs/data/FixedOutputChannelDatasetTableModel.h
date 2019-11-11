#ifndef FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H
#define FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H

#include "ChaosAbstractDataSetTableModel.h"

#include <QVector>
#include <QSharedPointer>
#include <QMimeData>
class FixedOutputChannelDatasetTableModel:
        public ChaosAbstractDataSetTableModel
{
    Q_OBJECT
public:
    FixedOutputChannelDatasetTableModel(const QString& node_uid,
                                        unsigned int dataset_type,
                                        QObject *parent = nullptr);
    ~FixedOutputChannelDatasetTableModel();

    void updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _dataset);
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    Qt::DropActions supportedDragActions() const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const ;
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
    bool isCellSelectable(const QModelIndex &index) const;
private:
    QString getSubTypeForCode(QSharedPointer<chaos::common::data::CDataWrapper> element) const;
};

#endif // FIXEDOUTPUTCHANNELDATASETTABLEMODEL_H
