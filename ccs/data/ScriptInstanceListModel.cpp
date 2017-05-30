#include "ScriptInstanceListModel.h"
#include "../metatypes.h"

#include <QMimeType>
#include <QMimeData>
#include <QDataStream>

using namespace chaos::common::data;
using namespace chaos::service_common::data::node;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy::script;

ScriptInstanceListModel::ScriptInstanceListModel(const ScriptBaseDescription& _script_description,
                                                 QObject *parent):
    ChaosAbstractTableModel(parent),
    script_description(_script_description),
    api_submitter(this) {}

void ScriptInstanceListModel::updateInstanceListForSearchString(const QString& _search_string) {
    search_string = _search_string;
    startSearch();
}

void ScriptInstanceListModel::commitChange() {
    std::vector<chaos::service_common::data::script::ScriptInstance> to_save;
    QSetIterator<unsigned int> it(changed_instance);
    while(it.hasNext()) {
        to_save.push_back(ni_list_wrapper[it.next()]);
    }

    if(to_save.size()) {
        api_submitter.submitApiResult("update_instances",
                                      GET_CHAOS_API_PTR(UpdateBindType)->execute(script_description,
                                                                                 to_save));
    }
}

void ScriptInstanceListModel::rollbackChange() {
    beginResetModel();
    ni_list_wrapper = original_ni_list_wrapper;
    endResetModel();
}

void ScriptInstanceListModel::startSearch() {
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
    if(tag.compare("update_instances") == 0) {
        emit commitDone(true);
    } else {
        beginResetModel();
        SearchInstancesForScript::deserialize(*api_result,
                                              ni_list_wrapper);

        changed_instance.clear();
        original_ni_list_wrapper = ni_list_wrapper;
        endResetModel();
    }
}

void ScriptInstanceListModel::onApiTimeout(const QString& tag) {
    if(tag.compare("update_instances") == 0) {
        emit commitDone(false);
    }
}

void ScriptInstanceListModel::onApiError(const QString& tag,
                                         QSharedPointer<chaos::CException> api_exception) {
    if(tag.compare("update_instances") == 0) {
        emit commitDone(false);
    }
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

bool ScriptInstanceListModel::isCellEditable(const QModelIndex &index) const {
    return index.column() == 1;
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
        return QVariant(QString::fromStdString(scriptBindTypeDecodeCode(ni_list_wrapper[row].bind_type)));
    case 2:
        return QString::fromStdString(ni_list_wrapper[row].bind_node);
    default:
        return "";
    }
}

QVariant ScriptInstanceListModel::getCellUserData(int row, int column) const {
    return QVariant::fromValue<chaos::service_common::data::script::ScriptInstance>(ni_list_wrapper[row]);
}

bool ScriptInstanceListModel::setCellData(const QModelIndex &index, const QVariant &value) {
    bool accepted = index.column() == 1;
    switch(index.column() ){
    case 0:
        break;
    case 1:
        changed_instance.insert(index.row());
        ni_list_wrapper[index.row()].bind_type = scriptBindTypeDecodeDescription(value.toString().toStdString());
        if(ni_list_wrapper[index.row()].bind_type != ScriptBindTypeUnitServer) {
            ni_list_wrapper[index.row()].bind_node.clear();
        }
        //signal the change on bind type
        emit bindTypeChanged(ni_list_wrapper[index.row()]);
        break;
    case 2:
        break;
    default:
        break;
    }
    return accepted;
}

Qt::ItemFlags ScriptInstanceListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = ChaosAbstractTableModel::flags(index);
    if (index.isValid()) {
        if (index.column() == 2 &&
                ni_list_wrapper[index.row()].bind_type == ScriptBindTypeUnitServer)
            return Qt::ItemIsDropEnabled | defaultFlags;
        else
            return defaultFlags;
    }
}

QStringList ScriptInstanceListModel::mimeTypes() const {
    QStringList types;
    types << "application/chaos-node-uid-type-list";
    return types;
}

bool ScriptInstanceListModel::canDropMimeData(const QMimeData *data,
                                              Qt::DropAction action,
                                              int row,
                                              int column,
                                              const QModelIndex &parent) {
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);
    int beginRow;

    if(column != 2) return false;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());

    if (!data->hasFormat("application/chaos-node-uid-type-list"))
        return false;
    if (ni_list_wrapper[beginRow].bind_type != ScriptBindTypeUnitServer)
        return false;
    return true;
}

bool ScriptInstanceListModel::dropMimeData(const QMimeData *data,
                                           Qt::DropAction action,
                                           int row,
                                           int column,
                                           const QModelIndex &parent) {
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    int beginRow;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());

    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("application/chaos-node-uid-type-list"))
        return false;
    QString node_uid;
    QString node_type;
    QByteArray encodedData = data->data("application/chaos-node-uid-type-list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    while (!stream.atEnd()) {
        stream >> node_uid;
        stream >> node_type;
        if(node_type.compare(chaos::NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
            changed_instance.insert(beginRow);
            ni_list_wrapper[beginRow].bind_node = node_uid.toStdString();
            //signal the change on bind type
            emit bindTypeChanged(ni_list_wrapper[beginRow]);
        }
    }
    return true;
}

Qt::DropActions ScriptInstanceListModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}
