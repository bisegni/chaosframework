#ifndef GROUPTREEMODEL_H
#define GROUPTREEMODEL_H

#include "GroupTreeItem.h"

#include <QString>
#include <QAbstractItemModel>

class GroupTreeModel: public QAbstractItemModel {
    Q_OBJECT

public:
    explicit GroupTreeModel(QObject *parent = 0);
    ~GroupTreeModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void loadRootsForDomain(const QString& domain);
private:
    GroupTreeItem *rootItem;
};
#endif // GRUOPTREEMODEL_H
