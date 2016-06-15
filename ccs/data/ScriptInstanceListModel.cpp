#include "ScriptInstanceListModel.h"

ScriptInstanceListModel::ScriptInstanceListModel(const chaos::service_common::data::script::ScriptBaseDescription &script_desc,
                                                 QObject *parent)
    : ChaosAbstractListModel(parent),
      api_submitter(this){
}


ScriptInstanceListModel::ScriptInstanceListModel(const chaos::service_common::data::script::ScriptBaseDescription& script_desc,
                                                 QObject *parent=0) {

}

void ScriptInstanceListModel::updateInstanceListForSearchString(const QString& search_tring) {

}

int ScriptInstanceListModel::getRowCount() const{

}

QVariant ScriptInstanceListModel::getRowData(int row) const{

}

QVariant ScriptInstanceListModel::getUserData(int row) const{

}

bool ScriptInstanceListModel::isRowCheckable(int row) const{

}

Qt::CheckState ScriptInstanceListModel::getCheckableState(int row)const{

}

bool ScriptInstanceListModel::setRowCheckState(const int row, const QVariant& value){

}

void ScriptInstanceListModel::onApiDone(const QString& tag,
                                        QSharedPointer<chaos::common::data::CDataWrapper> api_result){

}
