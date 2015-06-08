#ifndef CHAOSABSTRACTLISTMODEL_H
#define CHAOSABSTRACTLISTMODEL_H

#include <QAbstractListModel>

class ChaosAbstractListModel :
        public QAbstractListModel {
    Q_OBJECT

protected:
    virtual int getRowCount() const = 0;
    virtual QVariant getRowData(int row) const = 0;
    virtual QVariant getTooltipTextForData(int row) const;
public:
    ChaosAbstractListModel( QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

};

#endif // CHAOSABSTRACTLISTMODEL_H
