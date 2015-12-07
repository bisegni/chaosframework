/*
 *	ApiProxy.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>
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
mn_message(_mn_message){
    
}
//! default destructor
ApiProxy::~ApiProxy() {
    
}

//! execute an api call

ApiProxyResult ApiProxy::callApi(chaos::common::data::CDataWrapper *api_message) {
    CHAOS_ASSERT(mn_message)
    return  mn_message->sendRequestWithFuture(group_name,
                                              api_name,
                                              api_message,
                                              timeout_in_milliseconds);
}
