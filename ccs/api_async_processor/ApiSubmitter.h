#ifndef APISUBMITTER_H
#define APISUBMITTER_H

#include "ApiHandler.h"
#include "ApiAsyncProcessor.h"

#include <QObject>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class ApiSubmitter:
        public QObject {
    Q_OBJECT
public:
    ApiSubmitter(ApiHandler *_api_handler,
                 QObject *parent = 0);

public:
    //!submit api result for async wait termination
    void submitApiResult(const QString& api_tag,
                         chaos::metadata_service_client::api_proxy::ApiProxyResult api_result);
private slots:

    void asyncApiResult(const QString& tag,
                        QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void asyncApiError(const QString& tag,
                       QSharedPointer<chaos::CException> api_exception);

    void asyncApiTimeout(const QString& tag);

private:
    ApiHandler  *api_handler;
    ApiAsyncProcessor api_processor;
};

#endif // APISUBMITTER_H
