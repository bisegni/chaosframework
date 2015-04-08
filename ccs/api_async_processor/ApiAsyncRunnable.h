#ifndef APIASYNCRUNNABLE_H
#define APIASYNCRUNNABLE_H

#include <QObject>
#include <QRunnable>

#include <QSharedPointer>
#include <chaos/common/data/CDataWrapper.h>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>
class ApiAsyncRunnable :
        public QObject,
        public QRunnable
{
    Q_OBJECT
    const QString tag;
    chaos::metadata_service_client::api_proxy::ApiProxyResult async_result;
public:
    explicit ApiAsyncRunnable(const QString& _tag,
                              chaos::metadata_service_client::api_proxy::ApiProxyResult _async_result);
    ~ApiAsyncRunnable();
    void run();
signals:
    void asyncApiResult(QString tag, QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void asyncApiError(QString tag, QSharedPointer<chaos::CException> api_exception);
    void asyncApiTimeout(QString tag);
public slots:
};

#endif // APIASYNCRUNNABLE_H
