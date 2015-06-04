#ifndef CHAOSABSTRACTLISTMODEL_H
#define CHAOSABSTRACTLISTMODEL_H

#include <QAbstractListModel>

class ChaosAbstractListModel :
        public QAbstractListModel {
    Q_OBJECT

protected:
    virtual int getRowCount() const = 0;
    virtual QString getHeaderForSection(int section) const = 0;
    virtual QVariant getRowData(int row) const = 0;
    virtual QVariant getFontForData(int row) const;
    virtual QVariant getBackgroundForData(int row) const;
    virtual QVariant getTextColorForData(int row) const;
    virtual QVariant getTextAlignForData(int row) const;
    virtual QVariant getCheckStateForData(int row) const;
    virtual QVariant getTooltipTextForData(int row) const;
    virtual bool setCellData(const QModelIndex &index, const QVariant &value);
    virtual bool isCellEditable(const QModelIndex &index) const;
public:
    ChaosAbstractListModel( QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

};

#endif // CHAOSABSTRACTLISTMODEL_H
