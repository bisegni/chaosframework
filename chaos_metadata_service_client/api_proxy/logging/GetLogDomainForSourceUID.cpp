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

#include <chaos_metadata_service_client/api_proxy/logging/GetLogDomainForSourceUID.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::logging;

API_PROXY_CD_DEFINITION(GetLogDomainForSourceUID,
                        MetadataServerLoggingDefinitionKeyRPC::ACTION_NODE_LOGGING_RPC_DOMAIN,
                        "getLogDomainForSourceUID");

ApiProxyResult GetLogDomainForSourceUID::execute() {
    return callApi();
}

ApiProxyResult GetLogDomainForSourceUID::execute(const std::string& source) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> pack(new CDataWrapper());
    pack->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, source);
    return callApi(pack.release());
}

ApiProxyResult GetLogDomainForSourceUID::execute(const std::vector<std::string>& source) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> pack(new CDataWrapper());
    if(source.size()) {
        for(std::vector<std::string>::const_iterator it= source.begin();
            it!= source.end();
            it++) {
            pack->appendStringToArray(*it);
        }
        pack->finalizeArrayForKey(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER);
    }
    return callApi(pack.release());
}

ChaosUniquePtr<GetLogDomainForSourceUIDHelper> GetLogDomainForSourceUID::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<GetLogDomainForSourceUIDHelper>(new GetLogDomainForSourceUIDHelper(api_result));
}

GetLogDomainForSourceUIDHelper::GetLogDomainForSourceUIDHelper(CDataWrapper *api_result) {
    //now we have the result
    if(!api_result || !api_result->hasKey("result_list")) return;
    
    CMultiTypeDataArrayWrapperSPtr vec = api_result->getVectorValue("result_list");
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
