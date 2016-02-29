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
    return checked_index.testBit(row)?Qt::Checked:Qt::Unchecked;
}

bool LogDomainListModel::setRowCheckState(const int row, const QVariant& value) {
    //set checked state of the selected bit
    checked_index.setBit(row, value.toBool());
    return true;
}

void LogDomainListModel::onApiDone(const QString& tag,
                                   QSharedPointer<CDataWrapper> api_result) {
    //data received
    beginResetModel();
    helper = logging::GetLogDomainForSourceUID::getHelper(api_result.data());

    //resize the bit array for checked indexing
    checked_index.resize(helper->getLogDomainListSize());

    //fill all bit as enable
    checked_index.fill(false);

    endResetModel();

    //emit signal that model has changed
    emit(dataChanged(LogDomainListModel::index(0,0), LogDomainListModel::index(helper->getLogDomainListSize(),1)));
}

void LogDomainListModel::updateDomainListForUID(const QString &node_uid) {
    api_submitter.submitApiResult("LogDomainListModel::load_domain_list",
                                  GET_CHAOS_API_PTR(logging::GetLogDomainForSourceUID)->execute(node_uid.toStdString()));
}

void LogDomainListModel::updateDomainListForUID() {
    api_submitter.submitApiResult("LogDomainListModel::load_domain_list",
                                  GET_CHAOS_API_PTR(logging::GetLogDomainForSourceUID)->execute());
}

void LogDomainListModel::getActiveDomains(logging::LogDomainList& checked_domain) {
    for(int idx = 0;
        idx < checked_index.size();
        idx++) {
        if(checked_index.testBit(idx)) {
            //add domain
            checked_domain.push_back(helper->getLogDomainList()[idx]);
        }
    }
}

bool LogDomainListModel::isDomainChecked(const QString& domain_name) {
    for(int idx = 0;
        idx < helper->getLogDomainListSize();
        idx++) {
        if(helper->getLogDomainList()[idx].compare(domain_name.toStdString()) == 0) {
            //found it
            return checked_index.testBit(idx);
        }
    }
    return false;
}

bool LogDomainListModel::isDomainPresent(const QString& domain_name) {
    for(int idx = 0;
        idx < helper->getLogDomainListSize();
        idx++) {
        if(helper->getLogDomainList()[idx].compare(domain_name.toStdString()) == 0) {
            //found it
            return true;
        }
    }
    return false;
}
