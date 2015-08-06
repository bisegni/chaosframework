#include "ApiAsyncRunnable.h"
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;

ApiAsyncRunnable::ApiAsyncRunnable(const QString& _tag, ApiProxyResult _async_result):
    QObject(NULL),
    tag(_tag),
    async_result(_async_result)
    {setAutoDelete(true);}

ApiAsyncRunnable::~ApiAsyncRunnable(){}

void ApiAsyncRunnable::run() {
   if(async_result->wait()) {
        if(async_result->getError()) {
            //we have an error
            emit asyncApiError(tag, QSharedPointer<CException>(new CException(async_result->getError(),
                                                                              async_result->getErrorMessage(),
                                                                              async_result->getErrorDomain())));
        } else {
            //we have good answer
            emit asyncApiResult(tag, QSharedPointer<CDataWrapper>(async_result->detachResult()));
        }
    } else {
        //no data
        emit asyncApiTimeout(tag);
    }
}
