#ifndef APPLICATIONERRORLOGGINGTABLEMODEL_H
#define APPLICATIONERRORLOGGINGTABLEMODEL_H

#include "ChaosAbstractTableModel.h"

#include <QVector>
#include <QSharedPointer>

class ApplicationErrorLoggingTableModel:
        public ChaosAbstractTableModel {
    Q_OBJECT
public:
    ApplicationErrorLoggingTableModel(QObject *parent = 0);
    void updateLog();
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getUserData(int row, int column) const;
    bool isCellEditable(const QModelIndex &index) const;
    bool isCellCheckable(const QModelIndex &index) const;
protected slots:
    void onErrorEntryUpdated();
};

#endif // APPLICATIONERRORLOGGINGTABLEMODEL_H
