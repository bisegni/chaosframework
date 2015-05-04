#ifndef CHAOSABSTRACTNODETABLEMODEL_H
#define CHAOSABSTRACTNODETABLEMODEL_H

#include <QAbstractTableModel>

class ChaosAbstractNodeTableModel :
        public QAbstractTableModel {
    Q_OBJECT

protected:
    virtual int getRowCount() const = 0;
    virtual int getColumnCount() const = 0;
    virtual QString getHeaderForColumn(int column) const = 0;
    virtual QVariant getCellData(int row, int column) const;
    virtual QVariant getFontForData(int row, int column) const;
    virtual QVariant getBackgroundForData(int row, int column) const;
    virtual QVariant getTextAlignForData(int row, int column) const;
    virtual QVariant getCheckStateForData(int row, int column) const;
public:
    ChaosAbstractNodeTableModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif // CHAOSABSTRACTNODETABLEMODEL_H
