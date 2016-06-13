#include "DomainListModel.h"
using namespace chaos::metadata_service_client::api_proxy::groups;

DomainListModel::DomainListModel(QObject *parent):
    ChaosAbstractListModel(parent),
    ApiHandler(),
    api_submitter(this){

}

DomainListModel::~DomainListModel() {

}

int DomainListModel::getRowCount() const {
    return (get_domains_helper.get()!=NULL)?get_domains_helper->getDomainsListSize():0;
}

QVariant DomainListModel::getRowData(int row) const {
    return QString::fromStdString(get_domains_helper->getDomainsList()[row]);
}
QVariant DomainListModel::getUserData(int row) const {
    return getRowData(row);
}

void DomainListModel::update() {
    api_submitter.submitApiResult("get_domains",
                                  GET_CHAOS_API_PTR(GetDomains)->execute());
}

void DomainListModel::onApiDone(const QString& tag,
                                QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare("get_domains") == 0) {
        beginResetModel();
        get_domains_helper = GetDomains::getHelper(api_result.data());
        endResetModel();
        emit domainListUpdated();
    }
}
