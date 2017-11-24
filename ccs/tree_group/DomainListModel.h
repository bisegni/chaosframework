#ifndef DOMAINLISTMODEL_H
#define DOMAINLISTMODEL_H

#include "../data/ChaosAbstractListModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <QVector>

class DomainListModel :
        public ChaosAbstractListModel,
        protected ApiHandler {
    Q_OBJECT

protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
public:
    DomainListModel(QObject *parent = 0);
    ~DomainListModel();
    void update();

signals:
    void domainListUpdated();

private:
    ApiSubmitter api_submitter;
    ChaosUniquePtr<chaos::metadata_service_client::api_proxy::groups::GetDomainsHelper> get_domains_helper;
};

#endif // DOMAINLISTMODEL_H
