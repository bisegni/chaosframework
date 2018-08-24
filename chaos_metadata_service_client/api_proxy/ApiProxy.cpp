/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>
using namespace chaos::metadata_service_client::api_proxy;


ApiResultHelper::ApiResultHelper(chaos::common::data::CDataWrapper *_api_result):
api_result(_api_result){}

ApiResultHelper::~ApiResultHelper() {
    if(api_result) delete(api_result);
}

//! default constructor
ApiProxy::ApiProxy(const std::string& _group_name,
                   const std::string& _api_name,
                   chaos::common::message::MultiAddressMessageChannel *_mn_message,
                   int32_t _timeout_in_milliseconds):
group_name(_group_name),
api_name(_api_name),
access_counter(0),
timeout_in_milliseconds(_timeout_in_milliseconds),
mn_message(_mn_message){}
ApiProxy::~ApiProxy() {}

//! execute an api call

ApiProxyResult ApiProxy::callApi(const chaos::common::data::CDWUniquePtr& api_message) {
    CHAOS_ASSERT(mn_message)
    return  mn_message->sendRequestWithFuture(group_name,
                                              api_name,
                                              ChaosMoveOperator(api_message),
                                              timeout_in_milliseconds);
}
