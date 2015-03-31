#ifndef APIWORKER_H
#define APIWORKER_H

#include "AsyncWorker.h"
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class ApiWorker:
        public AsyncWorker
{
public:
    explicit ApiWorker(chaos::metadata_service_client::api_proxy::ApiProxyResult api_result):
        AsyncWorker(NULL) {

    }

    ~ApiWorker();
};

#endif // APIWORKER_H
