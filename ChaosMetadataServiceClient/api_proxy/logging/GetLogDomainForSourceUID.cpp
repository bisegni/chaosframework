/*
 *	GetLogDomainForSourceUID.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 17/02/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/logging/GetLogDomainForSourceUID.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::logging;

API_PROXY_CD_DEFINITION(GetLogDomainForSourceUID,
                        MetadataServerLoggingDefinitionKeyRPC::ACTION_NODE_LOGGING_RPC_DOMAIN,
                        "getLogDomainForSourceUID");

ApiProxyResult GetLogDomainForSourceUID::execute(const std::string& source) {
    std::auto_ptr<CDataWrapper> pack(new CDataWrapper());
    pack->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, source);
    return callApi(pack.release());
}

std::auto_ptr<GetLogDomainForSourceUIDHelper> GetLogDomainForSourceUID::getHelper(CDataWrapper *api_result) {
    return std::auto_ptr<GetLogDomainForSourceUIDHelper>(new GetLogDomainForSourceUIDHelper(api_result));
}

GetLogDomainForSourceUIDHelper::GetLogDomainForSourceUIDHelper(CDataWrapper *api_result) {
    //now we have the result
    if(!api_result || !api_result->hasKey("result_list")) return;
    
    std::auto_ptr<CMultiTypeDataArrayWrapper> vec(api_result->getVectorValue("result_list"));
    for(int idx = 0;
        idx < vec->size();
        idx++) {
        log_type_list.push_back(vec->getStringElementAtIndex(idx));
    }
}

GetLogDomainForSourceUIDHelper::~GetLogDomainForSourceUIDHelper() {}

size_t GetLogDomainForSourceUIDHelper::getLogDomainListSize() {
    return log_type_list.size();
}

const LogDomainList& GetLogDomainForSourceUIDHelper::getLogDomainList(){
    return log_type_list;
}