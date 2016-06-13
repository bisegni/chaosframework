#ifndef CHAOSABSTRACTLISTMODEL_H
#define CHAOSABSTRACTLISTMODEL_H

#include <QAbstractListModel>

class ChaosAbstractListModel :
        public QAbstractListModel {
    Q_OBJECT

protected:
    virtual int getRowCount() const = 0;
    virtual QVariant getRowData(int row) const = 0;
    virtual QVariant getUserData(int row) const = 0;
    virtual Qt::CheckState getCheckableState(int row)const;
    virtual bool isRowCheckable(int row) const;
    virtual bool isRowEditable(int row) const;
    virtual bool isRowSelectable(int row) const;
    virtual QVariant getTooltipTextForData(int row) const;
    virtual bool setRowData(const int row, const QVariant& value);
    virtual bool setRowCheckState(const int row, const QVariant& value);
public:
    ChaosAbstractListModel( QObject *parent = 0);
    ~ChaosAbstractListModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

};

#endif // CHAOSABSTRACTLISTMODEL_H
