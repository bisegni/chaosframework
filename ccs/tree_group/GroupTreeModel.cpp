#include "GroupTreeModel.h"

GroupTreeModel::GroupTreeModel(QObject *parent):
    QAbstractItemModel(parent){
    rootItem = new GroupTreeItem(tr("Name"), tr("root"));
}

GroupTreeModel::~GroupTreeModel() {
    delete rootItem;
}

QVariant GroupTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    GroupTreeItem *item = static_cast<GroupTreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags GroupTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant GroupTreeModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex GroupTreeModel::index(int row, int column,
                                  const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    GroupTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<GroupTreeItem*>(parent.internalPointer());

    GroupTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex GroupTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    GroupTreeItem *childItem = static_cast<GroupTreeItem*>(index.internalPointer());
    GroupTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int GroupTreeModel::rowCount(const QModelIndex &parent) const {
    GroupTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<GroupTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int GroupTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<GroupTreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

void GroupTreeModel::loadRootsForDomain(const QString& domain) {
    beginResetModel();
    rootItem->removeChild();
    endResetModel();
}
