#include "DomainListModel.h"
using namespace chaos::metadata_service_client::api_proxy::groups;

DomainListModel::DomainListModel(QObject *parent):
    ChaosAbstractListModel(parent) {

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

void DomainListModel::update(std::auto_ptr<GetDomainsHelper> _get_domains_helper) {
    beginResetModel();
    get_domains_helper = _get_domains_helper;
    endResetModel();
}
