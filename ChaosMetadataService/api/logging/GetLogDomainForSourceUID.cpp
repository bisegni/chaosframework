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

CHAOS_MDS_DEFINE_API_CLASS_CD(GetLogDomainForSourceUID, "getLogDomainForSourceUID")

CDWUniquePtr GetLogDomainForSourceUID::execute(CDWUniquePtr api_data) {
    int err = 0;
    CreateNewDataWrapper(result,);
    LogDomainList domain_list;
    
    GET_DATA_ACCESS(LoggingDataAccess, l_da, -4);
    
    std::vector<std::string> source_id_to_include;
    if(api_data &&
       api_data->hasKey(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER)) {
        //we have domain
        if(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER)) {
            source_id_to_include.push_back(api_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER));
        } else if(api_data->isVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER)) {
            CMultiTypeDataArrayWrapperSPtr domain_vec(api_data->getVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER));
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
        for(LogDomainListIterator it = domain_list.begin();
            it != domain_list.end();
            it++){
            result->appendStringToArray(*it);
        }
        result->finalizeArrayForKey("result_list");
    }
    return result;
}
