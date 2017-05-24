#include "ScriptInstanceListModel.h"

using namespace chaos::common::data;
using namespace chaos::service_common::data::node;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy::script;

inline QString getBindTypeSTR(ScriptBindType type) {
    switch(type){
    case ScriptBindTypeDisable: return "Disable";
    case ScriptBindTypeAuto: return "Auto";
    case ScriptBindTypeUnitServer: return "UnitServer";
    }
}

Q_DECLARE_METATYPE(chaos::service_common::data::script::ScriptInstance)
ScriptInstanceListModel::ScriptInstanceListModel(const ScriptBaseDescription& _script_description,
                                                 QObject *parent):
    ChaosAbstractTableModel(parent),
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

void ScriptInstanceListModel::onApiDone(const QString& tag,
                                        QSharedPointer<CDataWrapper> api_result){
    beginResetModel();
    SearchInstancesForScript::deserialize(*api_result,
                                          ni_list_wrapper);
    endResetModel();
}

int ScriptInstanceListModel::getRowCount() const {
    return ni_list_wrapper.size();
}

int ScriptInstanceListModel::getColumnCount() const {
    return 3;
}

bool ScriptInstanceListModel::isCellSelectable(const QModelIndex &index) const {
    return true;
}

QString ScriptInstanceListModel::getHeaderForColumn(int column) const {
    switch (column) {
        case 0:
            return "Instance Name";
        case 1:
            return "Bind Type";
        case 2:
            return "Bind Parent";
        default:
            return "";
        }
}

QVariant ScriptInstanceListModel::getCellData(int row, int column) const {
    switch (column) {
    case 0:
        return QString::fromStdString(ni_list_wrapper[row].instance_name);
    case 1:
        return getBindTypeSTR(ni_list_wrapper[row].bind_type);
    case 2:
        return QString::fromStdString(ni_list_wrapper[row].bind_node);
    default:
        return "";
    }
}

QVariant ScriptInstanceListModel::getCellUserData(int row, int column) const {
    return QVariant::fromValue<chaos::service_common::data::script::ScriptInstance>(ni_list_wrapper[row]);
}
