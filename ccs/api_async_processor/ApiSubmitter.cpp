#include "ApiSubmitter.h"

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;

ApiSubmitter::ApiSubmitter(ApiHandler *_api_handler, QObject *parent):
    QObject(parent),
    api_handler(_api_handler){}

//!submit api result for async wait termination
void ApiSubmitter::submitApiResult(const QString& api_tag,
                                   chaos::metadata_service_client::api_proxy::ApiProxyResult api_result) {
    CHAOS_ASSERT(api_handler)
            api_processor.submitApiResult(api_tag,
                                          std::move(api_result),
                                          (QObject*)this,
                                          SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                          SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                          SLOT(asyncApiTimeout(QString)));
    api_handler->apiHasStarted(api_tag);
}

void ApiSubmitter::asyncApiResult(const QString& tag,
                                  QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    CHAOS_ASSERT(api_handler)
    if(api_result == NULL) return;
    api_handler->onApiDone(tag,
                           api_result);
    //emit signal that we are finisched the wait on api result
    api_handler->apiHasEnded(tag);
}

void ApiSubmitter::asyncApiError(const QString& tag,
                                 QSharedPointer<chaos::CException> api_exception) {
    CHAOS_ASSERT(api_handler)
            api_handler->onApiError(tag,
                                    api_exception);
    api_handler->apiHasEndedWithError(tag,
                                      api_exception);
    api_handler->apiHasEnded(tag);
}

void ApiSubmitter::asyncApiTimeout(const QString& tag) {
    CHAOS_ASSERT(api_handler)
            api_handler->onApiTimeout(tag);
    api_handler->apiHasEndedWithError(tag,
                                      QSharedPointer<chaos::CException>(new chaos::CException(-1, "ApiTimeout", "ApiSubmitter::asyncApiTimeout")));
    api_handler->apiHasEnded(tag);
}
