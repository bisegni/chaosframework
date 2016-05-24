#include "ScriptListModel.h"

ScriptListModel::ScriptListModel(QObject *parent):
    ChaosAbstractListModel(parent),
    ApiHandler(),
    api_submitter(this){}

int ScriptListModel::getRowCount() const {
    return 0;
}

QVariant ScriptListModel::getRowData(int row) const {
    return QVariant();
}

QVariant ScriptListModel::getUserData(int row) const {
    return QVariant();
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

}
