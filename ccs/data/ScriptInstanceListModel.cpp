#include "ScriptInstanceListModel.h"

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy::script;

ScriptInstanceListModel::ScriptInstanceListModel(const ScriptBaseDescription& _script_description,
                                                 QObject *parent):
    ChaosAbstractListModel(parent),
    script_description(_script_description),
    api_submitter(this){
}

void ScriptInstanceListModel::updateInstanceListForSearchString(const QString& search_string) {
    api_submitter.submitApiResult("search_instances",
                                  GET_CHAOS_API_PTR(SearchInstanceForScript)->execute(script_description.name,
                                                                                      search_string.toStdString(),
                                                                                      0,
                                                                                      100));
}

int ScriptInstanceListModel::getRowCount() const{

}

QVariant ScriptInstanceListModel::getRowData(int row) const{

}

QVariant ScriptInstanceListModel::getUserData(int row) const{

}

void ScriptInstanceListModel::onApiDone(const QString& tag,
                                        QSharedPointer<CDataWrapper> api_result){

}
