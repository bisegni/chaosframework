#include "ScriptInstanceListModel.h"

using namespace chaos::common::data;
using namespace chaos::service_common::data::node;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy::script;

Q_DECLARE_METATYPE(chaos::service_common::data::node::NodeInstance)
ScriptInstanceListModel::ScriptInstanceListModel(const ScriptBaseDescription& _script_description,
                                                 QObject *parent):
    ChaosAbstractListModel(parent),
    script_description(_script_description),
    api_submitter(this){
}

void ScriptInstanceListModel::updateInstanceListForSearchString(const QString& search_string) {
    api_submitter.submitApiResult("search_instances",
                                  GET_CHAOS_API_PTR(SearchInstancesForScript)->execute(script_description.name,
                                                                                      search_string.toStdString(),
                                                                                      0,
                                                                                      100));
}

const ScriptBaseDescription& ScriptInstanceListModel::getScriptDescription() {
    return script_description;
}

int ScriptInstanceListModel::getRowCount() const{
    return ni_list_wrapper.dataWrapped().size();
}

QVariant ScriptInstanceListModel::getRowData(int row) const{
    return QString::fromStdString(ni_list_wrapper.dataWrapped()[row].instance_name);
}

QVariant ScriptInstanceListModel::getUserData(int row) const{
    return QVariant::fromValue<chaos::service_common::data::node::NodeInstance>(ni_list_wrapper.dataWrapped()[row]);
}

void ScriptInstanceListModel::onApiDone(const QString& tag,
                                        QSharedPointer<CDataWrapper> api_result){
    beginResetModel();
    ni_list_wrapper.deserialize(api_result.data());
    endResetModel();
}
