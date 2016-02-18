#ifndef LOGDOMAINLISTMODEL_H
#define LOGDOMAINLISTMODEL_H

#include "ChaosAbstractListModel.h"

#include "../api_async_processor/ApiSubmitter.h"

#include <QScopedPointer>

class LogDomainListModel:
        public ChaosAbstractListModel,
        protected ApiHandler {
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
    bool isRowCheckable(int row) const;
    Qt::CheckState getCheckableState(int row)const;
    bool setRowData(const int row, const QVariant& value);
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
public:
    LogDomainListModel(QObject *parent=0);
    void updateDomainListForUID(const QString& node_uid);
private:
    ApiSubmitter api_submitter;
    std::auto_ptr<chaos::metadata_service_client::api_proxy::logging::GetLogDomainForSourceUIDHelper> helper;
};

#endif // LOGDOMAINLISTMODEL_H
