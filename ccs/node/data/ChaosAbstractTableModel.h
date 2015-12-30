#ifndef CHAOSABSTRACTNODETABLEMODEL_H
#define CHAOSABSTRACTNODETABLEMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <chaos/common/data/CDataWrapper.h>

class ChaosAbstractTableModel :
        public QAbstractTableModel {
    Q_OBJECT

protected:
    virtual int getRowCount() const = 0;
    virtual int getColumnCount() const = 0;
    virtual QString getHeaderForColumn(int column) const = 0;
    virtual QVariant getCellData(int row, int column) const;
    virtual QVariant getCheckeable(int row, int column) const;
    virtual QVariant getFontForData(int row, int column) const;
    virtual QVariant getBackgroundForData(int row, int column) const;
    virtual QVariant getTextColorForData(int row, int column) const;
    virtual QVariant getTextAlignForData(int row, int column) const;
    virtual QVariant getCheckStateForData(int row, int column) const;
    virtual QVariant getTooltipTextForData(int row, int column) const;
    virtual bool setCellData(const QModelIndex &index, const QVariant &value);
    virtual bool isCellEditable(const QModelIndex &index) const;
    virtual bool isCellCheckable(const QModelIndex &index) const;
    virtual bool isCellSelectable(const QModelIndex &index) const;
public:
    ChaosAbstractTableModel(QObject *parent=0);
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
};

#endif // CHAOSABSTRACTNODETABLEMODEL_H
