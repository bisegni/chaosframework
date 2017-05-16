#include "ApiAsyncProcessor.h"
#include "ApiAsyncRunnable.h"
ApiAsyncProcessor::ApiAsyncProcessor() :
    QObject(NULL)
{
}

ApiAsyncProcessor::~ApiAsyncProcessor()
{
}

void ApiAsyncProcessor::submitApiResult(const QString& tag,
                                        chaos::metadata_service_client::api_proxy::ApiProxyResult async_result,
                                        const QObject *slot_parent_pointer,
                                        const char* slot_done,
                                        const char* slot_error,
                                        const char* slot_timeout) {
    //create runnable
    ApiAsyncRunnable *aar = new ApiAsyncRunnable(tag,
                                                 std::move(async_result));

    //! connect slot between target and runnable
    QObject::connect(aar, SIGNAL(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                     slot_parent_pointer, slot_done);
    QObject::connect(aar, SIGNAL(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                     slot_parent_pointer, slot_error);
    QObject::connect(aar, SIGNAL(asyncApiTimeout(QString)),
                     slot_parent_pointer, slot_timeout);

    //submit runnable
    QThreadPool::globalInstance()->start(aar);
}
