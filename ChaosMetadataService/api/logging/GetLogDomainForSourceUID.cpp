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

#include "GetLogDomainForSourceUID.h"

using namespace chaos::metadata_service::api::logging;

#define L_GLTFS_INFO INFO_LOG(GetLogDomainForSourceUID)
#define L_GLTFS_DBG  DBG_LOG(GetLogDomainForSourceUID)
#define L_GLTFS_ERR  ERR_LOG(GetLogDomainForSourceUID)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::metadata_service::api::logging;
using namespace chaos::metadata_service::persistence::data_access;

GetLogDomainForSourceUID::GetLogDomainForSourceUID():
AbstractApi("getLogDomainForSourceUID"){
}

GetLogDomainForSourceUID::~GetLogDomainForSourceUID() {
}

chaos::common::data::CDataWrapper *GetLogDomainForSourceUID::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    CDataWrapper *result = NULL;
    LogDomainList domain_list;
    
    GET_DATA_ACCESS(LoggingDataAccess, l_da, -4);
    
    std::vector<std::string> source_id_to_include;
    if(api_data &&
       api_data->hasKey(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER)) {
        //we have domain
        if(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER)) {
            source_id_to_include.push_back(api_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER));
        } else if(api_data->isVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER)) {
            std::unique_ptr<CMultiTypeDataArrayWrapper> domain_vec(api_data->getVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER));
            for(int idx = 0;
                idx < domain_vec->size();
                idx++){
                source_id_to_include.push_back(domain_vec->getStringElementAtIndex(idx));
            }
        } else {
            LOG_AND_TROW_FORMATTED(L_GLTFS_ERR, -5, "Source identifier key '%1%' need to be string or array of string",%MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER);
        }
        
    }
    
    if((err = l_da->getLogDomainsForSource(domain_list,
                                           source_id_to_include))) {
        LOG_AND_TROW(L_GLTFS_ERR, err, "Error searching log types");
    }
    if(domain_list.size()) {
        std::unique_ptr<CDataWrapper> tmp_result(new CDataWrapper());
        for(LogDomainListIterator it = domain_list.begin();
            it != domain_list.end();
            it++){
            tmp_result->appendStringToArray(*it);
        }
        tmp_result->finalizeArrayForKey("result_list");
        result = tmp_result.release();
    }
    return result;
}
