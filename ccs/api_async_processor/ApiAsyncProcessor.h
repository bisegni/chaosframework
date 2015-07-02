#ifndef APIASYNCPROCESSOR_H
#define APIASYNCPROCESSOR_H

#include <QObject>
#include <QRunnable>
#include <QThreadPool>

#include <chaos/common/data/CDataWrapper.h>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#define GET_CHAOS_API_PTR(api_name)\
    chaos::metadata_service_client::ChaosMetadataServiceClient::getInstance()->getApiProxy<api_name>()

class ApiAsyncProcessor:
        public QObject
{
    Q_OBJECT
public:
    explicit ApiAsyncProcessor();
    ~ApiAsyncProcessor();

public:
    //! submit the api result for asinc wait givin a tag and a slot where send the answer
    void submitApiResult(const QString& tag,
                         chaos::metadata_service_client::api_proxy::ApiProxyResult async_result,
                         const QObject *slot_parent_pointer,
                         const char* slot_done,
                         const char *slot_error,
                         const char *slot_timeout);

};
#endif // APIASYNCPROCESSOR_H

