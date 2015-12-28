#include "GroupTreeItem.h"

#include <QVector>
#include <QSharedPointer>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

GroupTreeItem::GroupTreeItem(const QString &item_name,
                             const QString &item_domain,
                             GroupTreeItem *parent):
    QObject(NULL),
    m_item_name(item_name),
    m_item_domain(item_domain),
    m_parent_item(parent) {
}

GroupTreeItem::~GroupTreeItem() {
    qDeleteAll(m_child_items);
}

void GroupTreeItem::appendChild(GroupTreeItem *child) {
    m_child_items.append(child);
}

GroupTreeItem *GroupTreeItem::child(int row) {
    return m_child_items.value(row);
}

int GroupTreeItem::childCount() const {
    return m_child_items.count();
}

int GroupTreeItem::columnCount() const {
    return 1;
}

QVariant GroupTreeItem::data(int column) const {
    return m_item_name;
}

int GroupTreeItem::row() const {
    if (m_parent_item)
        return m_parent_item->m_child_items.indexOf(const_cast<GroupTreeItem*>(this));

    return 0;
}

GroupTreeItem *GroupTreeItem::parentItem() {
    return m_parent_item;
}

void GroupTreeItem::removeChild() {
    qDeleteAll(m_child_items);
    m_child_items.clear();
}

QString GroupTreeItem::getPathToRoot() {
    QString path="/";
    QVector<QString> element_in_path;
    element_in_path.push_front(m_item_name);

    GroupTreeItem *parent = m_parent_item;
    while(parent) {
        //we need to remove the root thatis a fake element and not give us an element of the path
        if(parent->parentItem() == NULL) break;
        // add the name of the parent
        element_in_path.push_front(parent->m_item_name);
    }
    if(element_in_path.size()){
        //append all element of the path to the string
        QVectorIterator<QString> element_in_path_it(element_in_path);
        while(element_in_path_it.hasNext()) {
            path.append(element_in_path_it.next());
            path.append("/");
        }
        //remove last slash
        path.resize(path.size()-1);
    }
    return path;
}

const QString& GroupTreeItem::getDomain() {
    return m_item_domain;
}
