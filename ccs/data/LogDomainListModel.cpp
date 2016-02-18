#include "LogDomainListModel.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;

LogDomainListModel::LogDomainListModel(QObject *parent):
    ChaosAbstractListModel(parent),
    ApiHandler(),
    api_submitter(this){

}

int LogDomainListModel::getRowCount() const {
    return helper.get()?helper->getLogDomainListSize():0;
}

QVariant LogDomainListModel::getRowData(int row) const {
    return QString::fromStdString(helper->getLogDomainList()[row]);
}

QVariant LogDomainListModel::getUserData(int row) const {
    return getRowData(row);
}

bool LogDomainListModel::isRowCheckable(int row) const {
    return true;
}

Qt::CheckState LogDomainListModel::getCheckableState(int row)const {
    return Qt::Checked;
}

bool LogDomainListModel::setRowData(const int row, const QVariant& value) {
    return true;
}

void LogDomainListModel::onApiDone(const QString& tag,
                                   QSharedPointer<CDataWrapper> api_result) {
    //data received
    beginResetModel();
    helper = logging::GetLogDomainForSourceUID::getHelper(api_result.data());
    endResetModel();
}

void LogDomainListModel::updateDomainListForUID(const QString &node_uid) {
    api_submitter.submitApiResult("load_domain_list",
                                  GET_CHAOS_API_PTR(logging::GetLogDomainForSourceUID)->execute(node_uid.toStdString()));
}
