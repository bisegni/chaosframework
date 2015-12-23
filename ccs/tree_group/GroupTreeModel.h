#ifndef GROUPTREEMODEL_H
#define GROUPTREEMODEL_H

#include "GroupTreeItem.h"

#include <QMap>
#include <QString>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QAbstractItemModel>

#include "../api_async_processor/ApiAsyncProcessor.h"

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
    void addNewNodeToIndex(const QModelIndex &node_parent, QString node_name);
protected slots:
    void asyncApiResult(const QString& tag,
                        QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void asyncApiError(const QString& tag,
                       QSharedPointer<chaos::CException> api_exception);

    void asyncApiTimeout(const QString& tag);

private:
    QMap<QString, QModelIndex> model_index_load_child_map;
    QReadWriteLock read_write_lock;
    ApiAsyncProcessor api_processor;
    GroupTreeItem *rootItem;
};
#endif // GRUOPTREEMODEL_H
