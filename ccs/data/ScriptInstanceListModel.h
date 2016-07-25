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
    ScriptInstanceListModel(const chaos::service_common::data::script::ScriptBaseDescription& _script_description,
                            QObject *parent=0);
    void updateInstanceListForSearchString(const QString& search_string);

    const chaos::service_common::data::script::ScriptBaseDescription& getScriptDescription();
private:
    ApiSubmitter api_submitter;
    //description of the script parent of the found instance
    chaos::service_common::data::script::ScriptBaseDescription script_description;
    //list of the current pag contained the found instances
    chaos::common::data::structured::StdVectorSDWrapper<chaos::service_common::data::node::NodeInstance,
    chaos::service_common::data::node::NodeInstanceSDWrapper> ni_list_wrapper;
};

#endif // SCRIPTINSTANCELISTMODEL_H
