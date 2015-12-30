#ifndef DOMAINLISTMODEL_H
#define DOMAINLISTMODEL_H

#include "../node/data/ChaosAbstractListModel.h"

#include <QVector>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class DomainListModel :
        public ChaosAbstractListModel {

protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;

public:
    DomainListModel(QObject *parent = 0);
    void update(std::auto_ptr<chaos::metadata_service_client::api_proxy::groups::GetDomainsHelper> _get_domains_helper);
private:
    std::auto_ptr<chaos::metadata_service_client::api_proxy::groups::GetDomainsHelper> get_domains_helper;
};

#endif // DOMAINLISTMODEL_H
