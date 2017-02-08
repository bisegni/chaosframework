#ifndef AGENTNODEASSOCIATEDLISTMODEL_H
#define AGENTNODEASSOCIATEDLISTMODEL_H

#include "ChaosAbstractListModel.h"
#include "../api_async_processor/ApiSubmitter.h"

class AgentNodeAssociatedListModel:
        public ChaosAbstractListModel,
        protected ApiHandler {

public:
    AgentNodeAssociatedListModel();
    void setAgent(const QString& agent_uid);
protected:
    virtual int getRowCount() const = 0;
    virtual QVariant getRowData(int row) const = 0;
    virtual QVariant getUserData(int row) const = 0;
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private:
    ApiSubmitter api_submitter;
};

#endif // AGENTNODEASSOCIATEDLISTMODEL_H
