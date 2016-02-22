#ifndef LOGDOMAINLISTMODEL_H
#define LOGDOMAINLISTMODEL_H

#include "ChaosAbstractListModel.h"

#include "../api_async_processor/ApiSubmitter.h"

#include <QBitArray>

class LogDomainListModel:
        public ChaosAbstractListModel,
        protected ApiHandler {
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
    bool isRowCheckable(int row) const;
    Qt::CheckState getCheckableState(int row)const;
    bool setRowCheckState(const int row, const QVariant& value);
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
public:
    LogDomainListModel(QObject *parent=0);
    //update the list with all the domain emitted by the uid
    void updateDomainListForUID(const QString& node_uid);
    //!return all checked domain
    void getActiveDomains(chaos::metadata_service_client::api_proxy::logging::LogDomainList &checked_domain);
    //! check if a domain is selected
    bool isDomainChecked(const QString& domain_name);
private:
    QBitArray checked_index;
    ApiSubmitter api_submitter;
    std::auto_ptr<chaos::metadata_service_client::api_proxy::logging::GetLogDomainForSourceUIDHelper> helper;
};

#endif // LOGDOMAINLISTMODEL_H
