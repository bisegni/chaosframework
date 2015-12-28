#ifndef GROUPTREEITEM_H
#define GROUPTREEITEM_H

#include <QList>
#include <QObject>
#include <QVariant>

#include "../api_async_processor/ApiAsyncProcessor.h"

class GroupTreeItem:
public QObject {
public:
    explicit GroupTreeItem(const QString &item_name,
                           const QString &item_domain,
                           GroupTreeItem *parentItem = 0);
    ~GroupTreeItem();

    void appendChild(GroupTreeItem *child);

    GroupTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    GroupTreeItem *parentItem();
    void removeChild();
    QString getPathToRoot();
    const QString& getDomain();

private:
    ApiAsyncProcessor api_processor;
    QList<GroupTreeItem*> m_child_items;
    const QString m_item_name;
    const QString m_item_domain;
    GroupTreeItem *m_parent_item;
};
#endif // GROUPTREEITEM_H
