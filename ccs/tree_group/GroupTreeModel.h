#ifndef GROUPTREEMODEL_H
#define GROUPTREEMODEL_H

#include "GroupTreeItem.h"

#include <QMap>
#include <QString>
#include <QMutex>
#include <QSharedPointer>
#include <QAbstractItemModel>

#include "../api_async_processor/ApiAsyncProcessor.h"

class GroupTreeModel: public QAbstractItemModel {
    Q_OBJECT

public:
    explicit GroupTreeModel(QObject *parent = 0);
    ~GroupTreeModel();

    void clear();
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
    const QString& currentDomain();
    void addNewRoot(QString node_name);
    void addNewNodeToIndex(const QModelIndex &node_parent, QString node_name);
    void updateNodeChildList(const QModelIndex &node_parent) const;
    void deleteNode(const QModelIndex &node_to_delete_index);
protected slots:
    void asyncApiResult(const QString& tag,
                        QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void asyncApiError(const QString& tag,
                       QSharedPointer<chaos::CException> api_exception);

    void asyncApiTimeout(const QString& tag);

    void _updateNodeChildList(const QModelIndex &parent,
                              const chaos::metadata_service_client::api_proxy::groups::NodeChildList& child_list);
    GroupTreeItem *getNewNode(const QString& node_name,
                              const QString& node_domain,
                              GroupTreeItem *parent_item);
private:
    mutable QMap<QString, QModelIndex> model_index_load_child_map;
    mutable QMutex mutex_update_model;
    mutable ApiAsyncProcessor api_processor;
    GroupTreeItem *root_item;
    QString current_domain;
};
#endif // GRUOPTREEMODEL_H
