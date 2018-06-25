#include "ScriptListModel.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;

ScriptListModel::ScriptListModel(QObject *parent):
    ChaosAbstractListModel(parent),
    ApiHandler(),
    api_submitter(this),
    last_sequence_id(0){
    script_base_list_wrapper.instance_serialization_key = chaos::MetadataServerApiKey::script::search_script::FOUND_SCRIPT_LIST;
}

int ScriptListModel::getRowCount() const {
    return script_base_list_wrapper.size();
}

QVariant ScriptListModel::getRowData(int row) const {
    //as row data return the script name
    return QString::fromStdString(script_base_list_wrapper[row].name);
}

QVariant ScriptListModel::getUserData(int row) const {
    //as user data return the unique id
    return  QVariant::fromValue<uint64_t>(script_base_list_wrapper[row].unique_id);
}

bool ScriptListModel::isRowCheckable(int row) const {
    return false;
}

Qt::CheckState ScriptListModel::getCheckableState(int row)const {
    return Qt::Unchecked;
}

bool ScriptListModel::setRowCheckState(const int row, const QVariant& value) {
    return false;
}

void ScriptListModel::onApiDone(const QString& tag,
                                QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    beginResetModel();
    //deserialize the result
    script_base_list_wrapper.deserialize(api_result.data());
    //manage sequence id
    endResetModel();
    //emit signal that model has changed
    emit(dataChanged(ScriptListModel::index(0,0), ScriptListModel::index(script_base_list_wrapper.size(),1)));
}

void ScriptListModel::updateSearchString(const QString& new_search_string) {
    search_string = new_search_string;
    last_sequence_id = 0;
    updateSearch();
}

void ScriptListModel::updateSearch() {
    api_submitter.submitApiResult("LogEntryTableModel::load_entry_list",
                                  GET_CHAOS_API_PTR(script::SearchScript)->execute(search_string.toStdString(),
                                                                                   last_sequence_id,
                                                                                   100));
}
