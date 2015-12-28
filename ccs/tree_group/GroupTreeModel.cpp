#include "GroupTreeModel.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QUuid>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

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

void GroupTreeModel::asyncApiResult(const QString& tag,
                                    QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.startsWith("domain>")) {
        QString domain = tag.split(">").back();
        std::auto_ptr<groups::GetNodeChildsHelper> gnc_helper = groups::GetNodeChilds::getHelper(api_result.data());
        beginResetModel();
        for(groups::NodeChildListConstIterator it = gnc_helper->getNodeChildsList().begin();
            it != gnc_helper->getNodeChildsList().end();
            it++) {
            rootItem->appendChild(new GroupTreeItem(QString::fromStdString(*it), domain, rootItem));
        }
        endResetModel();
    } else if(tag.startsWith("_add_new_node_")) {
        mutex_update_model.lock();
        QModelIndex node_index_parent = model_index_load_child_map[tag];
        model_index_load_child_map.remove(tag);
        mutex_update_model.unlock();

        //update list for parent node of the new added node
        updateNodeChildList(node_index_parent);
    } else if(tag.startsWith("_update_node_child_") &&
              api_result.data() != NULL) {
        mutex_update_model.lock();
        QModelIndex node_index_parent = model_index_load_child_map[tag];
        model_index_load_child_map.remove(tag);
        mutex_update_model.unlock();

        //get child list and call update param
        std::auto_ptr<groups::GetNodeChildsHelper> gnc_helper = groups::GetNodeChilds::getHelper(api_result.data());
        _updateNodeChildList(node_index_parent, gnc_helper->getNodeChildsList());
    }
}

void GroupTreeModel::asyncApiError(const QString& tag,
                                   QSharedPointer<chaos::CException> api_exception) {
    if(tag.startsWith("_add_new_node_")) {
            QString path = tag.split(">").front();
            model_index_load_child_map.remove(path);
        }
}

void GroupTreeModel::asyncApiTimeout(const QString& tag) {
    if(tag.startsWith("_add_new_node_")) {
            QString path = tag.split(">").front();
            model_index_load_child_map.remove(path);
        }
}

void GroupTreeModel::clear() {
    mutex_update_model.lock();
    beginResetModel();
    rootItem->removeChild();
    current_domain.clear();
    endResetModel();
    mutex_update_model.unlock();
}

void GroupTreeModel::loadRootsForDomain(const QString& domain) {
    mutex_update_model.lock();
    beginResetModel();
    rootItem->removeChild();
    current_domain = tag;
    api_processor.submitApiResult("domain>"+current_domain,
                                  GET_CHAOS_API_PTR(groups::GetNodeChilds)->execute(domain.toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
    endResetModel();
    mutex_update_model.unlock();
}

void GroupTreeModel::updateNodeChildList(const QModelIndex &node_parent) {
    if(!node_parent.isValid()) return;
    GroupTreeItem *parent_item = static_cast<GroupTreeItem*>(node_parent.internalPointer());
    QString tag = QString("_update_node_child_%1").arg(QUuid::createUuid().toString());
    model_index_load_child_map.insert(tag, node_parent);
    api_processor.submitApiResult(tag,
                                  GET_CHAOS_API_PTR(groups::GetNodeChilds)->execute(current_domain.toStdString(),
                                                                                    parent_item->getPathToRoot().toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
}

void GroupTreeModel::_updateNodeChildList(const QModelIndex &parent,
                                        const groups::NodeChildList& child_list) {
    GroupTreeItem *parentItem = static_cast<GroupTreeItem*>(node_index.internalPointer());
    if(parentItem == NULL) return;
    mutex_update_model.lock();
    beginInsertRows(node_index, 0, gnc_helper->getNodeChildsListSize()-1);
    for(groups::NodeChildListConstIterator it = child_list.begin();
        it != child_list.end();
        it++) {
        parentItem->appendChild(new GroupTreeItem(QString::fromStdString(*it),
                                                  parentItem->getDomain(),
                                                  parentItem));
    }
    endInsertRows();
    mutex_update_model.unlock();
}

void GroupTreeModel::addNewNodeToIndex(const QModelIndex &node_parent, QString node_name) {
    if(!node_parent.isValid()) return;
    GroupTreeItem *parentItem = static_cast<GroupTreeItem*>(node_parent.internalPointer());
    QString tag = QString("_add_new_node_%1").arg(QUuid::createUuid().toString());
    model_index_load_child_map.insert(tag, node_parent);
    api_processor.submitApiResult(tag,
                                  GET_CHAOS_API_PTR(groups::AddNode)->execute(parentItem->getDomain().toStdString(),
                                                                                    node_name.toStdString(),
                                                                                    parentItem->getPathToRoot().toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
}
