#ifndef AGENTNODEASSOCIATEDLISTMODEL_H
#define AGENTNODEASSOCIATEDLISTMODEL_H

#include "ChaosAbstractListModel.h"
#include "../api_async_processor/ApiSubmitter.h"
#include <chaos_service_common/data/node/Agent.h>

class AgentNodeAssociatedListModel:
        public ChaosAbstractListModel,
        protected ApiHandler {

public:
    AgentNodeAssociatedListModel();
    void setAgent(const QString& _agent_uid);
    void updateList();
protected:
    virtual int getRowCount() const;
    virtual QVariant getRowData(int row) const;
    virtual QVariant getUserData(int row) const;
    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action, int row, int column, const QModelIndex &parent);
    bool canDropMimeData(const QMimeData *data,
                         Qt::DropAction action, int row, int column, const QModelIndex &parent);
private:
    QString agent_uid;
    ApiSubmitter api_submitter;
    mutable chaos::service_common::data::agent::VectorAgentAssociationStatus associated_nodes;
};

#endif // AGENTNODEASSOCIATEDLISTMODEL_H
