#include "GroupTreeModel.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QUuid>
#include <QDebug>
#include <QMessageBox>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

GroupTreeModel::GroupTreeModel(QObject *parent):
    QAbstractItemModel(parent){
    root_item = new GroupTreeItem(tr("Name"), tr("root"));
}

GroupTreeModel::~GroupTreeModel() {
    delete root_item;
}

QVariant GroupTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    GroupTreeItem *item = static_cast<GroupTreeItem*>(index.internalPointer());
    if(item->fake_for_load) {
        //dinamically load possible child asynchronously
        updateNodeChildList(parent(index));
    }
    return item->data(index.column());
}

Qt::ItemFlags GroupTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant GroupTreeModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return root_item->data(section);

    return QVariant();
}

QModelIndex GroupTreeModel::index(int row, int column,
                                  const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    GroupTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = root_item;
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

    if (parentItem == root_item)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int GroupTreeModel::rowCount(const QModelIndex &parent) const {
    GroupTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = root_item;
    else
        parentItem = static_cast<GroupTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int GroupTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<GroupTreeItem*>(parent.internalPointer())->columnCount();
    else
        return root_item->columnCount();
}

void GroupTreeModel::asyncApiResult(const QString& tag,
                                    QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.startsWith("domain>")) {
        QString domain = tag.split(">").back();
        std::auto_ptr<groups::GetNodeChildsHelper> gnc_helper = groups::GetNodeChilds::getHelper(api_result.data());
        beginResetModel();
        root_item->removeChild();
        for(groups::NodeChildListConstIterator it = gnc_helper->getNodeChildsList().begin();
            it != gnc_helper->getNodeChildsList().end();
            it++) {
            root_item->appendChild(getNewNode(QString::fromStdString(*it), domain, root_item));
        }
        endResetModel();
    } else if(tag.startsWith("_add_new_node_")) {
        mutex_update_model.lock();
        QModelIndex node_index_parent = model_index_load_child_map[tag];
        model_index_load_child_map.remove(tag);
        mutex_update_model.unlock();

        //update list for parent node of the new added node
        updateNodeChildList(node_index_parent);
    } else if(tag.compare("_add_new_root_") == 0) {
        //reload all domain root
        api_processor.submitApiResult("domain>"+current_domain,
                                      GET_CHAOS_API_PTR(groups::GetNodeChilds)->execute(current_domain.toStdString()),
                                      this,
                                      SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                      SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                      SLOT(asyncApiTimeout(QString)));
    } else if(tag.startsWith("_update_node_child_")) {
        mutex_update_model.lock();
        QModelIndex node_index_parent = model_index_load_child_map[tag];
        model_index_load_child_map.remove(tag);
        mutex_update_model.unlock();

        //get child list and call update param
        std::auto_ptr<groups::GetNodeChildsHelper> gnc_helper = groups::GetNodeChilds::getHelper(api_result.data());
        _updateNodeChildList(node_index_parent, gnc_helper->getNodeChildsList());
    } else if(tag.startsWith("_delete_node_")) {
        mutex_update_model.lock();
        QModelIndex node_deleted_index = model_index_load_child_map[tag];
        model_index_load_child_map.remove(tag);
        mutex_update_model.unlock();
        //update parent
        updateNodeChildList(parent(node_deleted_index));
    }
}

void GroupTreeModel::asyncApiError(const QString& tag,
                                   QSharedPointer<chaos::CException> api_exception) {
    mutex_update_model.lock();
    model_index_load_child_map.remove(tag);
    mutex_update_model.unlock();
    QMessageBox::information(NULL,
                             tr("Api Error"),
                             tr(api_exception->what()));
}

void GroupTreeModel::asyncApiTimeout(const QString& tag) {
    mutex_update_model.lock();
    model_index_load_child_map.remove(tag);
    mutex_update_model.unlock();
    QMessageBox::information(NULL,
                             tr("Api Error"),
                             tr("Timeout"));
}

void GroupTreeModel::clear() {
    mutex_update_model.lock();
    beginResetModel();
    root_item->removeChild();
    current_domain.clear();
    endResetModel();
    mutex_update_model.unlock();
}

void GroupTreeModel::loadRootsForDomain(const QString& domain) {
    mutex_update_model.lock();
    beginResetModel();
    root_item->removeChild();
    current_domain = domain;
    api_processor.submitApiResult("domain>"+current_domain,
                                  GET_CHAOS_API_PTR(groups::GetNodeChilds)->execute(current_domain.toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
    endResetModel();
    mutex_update_model.unlock();
}

const QString& GroupTreeModel::currentDomain() {
    return current_domain;
}

void GroupTreeModel::updateNodeChildList(const QModelIndex &node_parent) const {
    if(!node_parent.isValid()) return;
    GroupTreeItem *parent_item = static_cast<GroupTreeItem*>(node_parent.internalPointer());
    QString tag = QString("_update_node_child_%1").arg(QUuid::createUuid().toString());

    mutex_update_model.lock();
    model_index_load_child_map.insert(tag, node_parent);
    mutex_update_model.unlock();

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
    GroupTreeItem *parentItem = static_cast<GroupTreeItem*>(parent.internalPointer());
    if(parentItem == NULL) return;

    mutex_update_model.lock();
    //check if we need to add or remove
    bool need_to_add = child_list.size() >=  parentItem->childCount();
    if(child_list.size()) {
        if(need_to_add) {
            beginInsertRows(parent, 0, child_list.size()-1);
        }else {
            beginRemoveColumns(parent, 0, child_list.size()-1);
        }
        parentItem->removeChild();
        for(groups::NodeChildListConstIterator it = child_list.begin();
            it != child_list.end();
            it++) {
            parentItem->appendChild(getNewNode(QString::fromStdString(*it),
                                               parentItem->getDomain(),
                                               parentItem));
        }
        if(need_to_add) {
            endInsertRows();
        }else {
            endRemoveRows();
        }
    } else {

        beginRemoveColumns(parent, 0, 0);
        parentItem->removeChild();
        endRemoveColumns();

    }
    mutex_update_model.unlock();
}

GroupTreeItem *GroupTreeModel::getNewNode(const QString& node_name,
                                          const QString& node_domain,
                                          GroupTreeItem *parent_item) {
    GroupTreeItem *new_node = new GroupTreeItem(node_name, node_domain, parent_item);
    GroupTreeItem *fake_item = new GroupTreeItem(tr("loading..."), tr(""), new_node);
    fake_item->fake_for_load = true;
    new_node->appendChild(fake_item);
    return new_node;
}

void GroupTreeModel::addNewRoot(QString node_name) {
    api_processor.submitApiResult("_add_new_root_",
                                  GET_CHAOS_API_PTR(groups::AddNode)->execute(current_domain.toStdString(),
                                                                              node_name.toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
}

void GroupTreeModel::addNewNodeToIndex(const QModelIndex &node_parent, QString node_name) {
    if(!node_parent.isValid()) return;
    GroupTreeItem *parentItem = static_cast<GroupTreeItem*>(node_parent.internalPointer());
    QString tag = QString("_add_new_node_%1").arg(QUuid::createUuid().toString());
    mutex_update_model.lock();
    model_index_load_child_map.insert(tag, node_parent);
    mutex_update_model.unlock();
    api_processor.submitApiResult(tag,
                                  GET_CHAOS_API_PTR(groups::AddNode)->execute(parentItem->getDomain().toStdString(),
                                                                              node_name.toStdString(),
                                                                              parentItem->getPathToRoot().toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
}

void GroupTreeModel::deleteNode(const QModelIndex &node_to_delete_index) {
    if(!node_to_delete_index.isValid()) return;
    GroupTreeItem *node_to_delete = static_cast<GroupTreeItem*>(node_to_delete_index.internalPointer());
    QString tag = QString("_delete_node_%1").arg(QUuid::createUuid().toString());
    QString parent_path = (node_to_delete->m_parent_item!=NULL)?node_to_delete->m_parent_item->getPathToRoot():"";
    mutex_update_model.lock();
    model_index_load_child_map.insert(tag, node_to_delete_index);
    mutex_update_model.unlock();
    api_processor.submitApiResult(tag,
                                  GET_CHAOS_API_PTR(groups::DeleteNode)->execute(node_to_delete->m_item_domain.toStdString(),
                                                                                 node_to_delete->m_item_name.toStdString(),
                                                                                 parent_path.toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
}
