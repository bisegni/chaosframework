#ifndef NODELISTMODEL_H
#define NODELISTMODEL_H

#include "../node/data/ChaosAbstractListModel.h"

#include <QVector>

class NodeListModel :
        public ChaosAbstractListModel {

protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;

public:
    NodeListModel(QObject *parent = 0);
    void addNode(const QString& new_node);
    void removeNode(const QString& new_node);
    const QVector<QString>& getAllNode();
private:
    QVector<QString> list_node_uid;
};

#endif // NODELISTMODEL_H
