#ifndef SCRIPTINSTANCELISTMODEL_H
#define SCRIPTINSTANCELISTMODEL_H

#include "ChaosAbstractListModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class ScriptInstanceListModel :
        public ChaosAbstractListModel,
        protected ApiHandler {
    Q_OBJECT
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
public:
    ScriptInstanceListModel(const chaos::service_common::data::script::ScriptBaseDescription& script_desc,
                            QObject *parent=0);
    //update the list with all the domain emitted by source uid
    void updateInstanceListForSearchString(const QString& search_tring);
private:
    ApiSubmitter api_submitter;
    chaos::service_common::data::script::ScriptBaseDescription script_description;
};

#endif // SCRIPTINSTANCELISTMODEL_H
