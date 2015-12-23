#ifndef GROUPTREEITEM_H
#define GROUPTREEITEM_H

#include <QList>
#include <QObject>
#include <QVariant>

#include "../api_async_processor/ApiAsyncProcessor.h"

class GroupTreeItem:
public QObject {
    Q_OBJECT
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
    void updatChild();
    const QString& getDomain();
protected slots:
    void asyncApiResult(const QString& tag,
                        QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void asyncApiError(const QString& tag,
                       QSharedPointer<chaos::CException> api_exception);

    void asyncApiTimeout(const QString& tag);

public slots:

private:
    ApiAsyncProcessor api_processor;
    QList<GroupTreeItem*> m_child_items;
    const QString m_item_name;
    const QString m_item_domain;
    GroupTreeItem *m_parent_item;
};
#endif // GROUPTREEITEM_H
