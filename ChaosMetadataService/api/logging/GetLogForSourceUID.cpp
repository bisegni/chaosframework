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

#include "GetLogForSourceUID.h"
#include "../../common/LogUtility.h"

#define L_GLFNI_INFO INFO_LOG(GetLogForSourceUID)
#define L_GLFNI_DBG  DBG_LOG(GetLogForSourceUID)
#define L_GLFNI_ERR  ERR_LOG(GetLogForSourceUID)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::api::logging;
using namespace chaos::metadata_service::persistence::data_access;

GetLogForSourceUID::GetLogForSourceUID():
AbstractApi("getLogForNodeUID"){}

GetLogForSourceUID::~GetLogForSourceUID() {}

CDWUniquePtr GetLogForSourceUID::execute(CDWUniquePtr api_data) {
    int err = 0;
    CreateNewDataWrapper(result,);
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, L_GLFNI_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, L_GLFNI_ERR, -2, "The log timestamp key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER), L_GLFNI_ERR, -3, "The log timestamp key needs to be a string value");
    
    GET_DATA_ACCESS(LoggingDataAccess, l_da, -4);
    
    //entry list
    LogEntryList entry_list;
    std::vector<std::string> domain_to_include;
    
    uint32_t page_length =  (uint32_t)CDW_GET_INT32_WITH_DEFAULT(api_data, "page_length", 100);
    uint64_t sequence = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(api_data, "seq", 0);
    if(api_data->hasKey(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
        //we have domain
        if(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
            domain_to_include.push_back(api_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN));
        } else if(api_data->isVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
            CMultiTypeDataArrayWrapperSPtr domain_vec(api_data->getVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN));
            for(int idx = 0;
                idx < domain_vec->size();
                idx++){
                if(domain_vec->isStringElementAtIndex(idx)) {
                    domain_to_include.push_back(domain_vec->getStringElementAtIndex(idx));
                }
            }
        } else {
            LOG_AND_TROW_FORMATTED(L_GLFNI_ERR, -5, "Domain key '%1% 'need to be string or array of string",%MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN);
        }
        
    }
    const std::string source = CDW_GET_SRT_WITH_DEFAULT(api_data, MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, "");
    
    if((err = l_da->searchEntryForSource(entry_list,
                                         source,
                                         domain_to_include,
                                         sequence,
                                         page_length))) {
        LOG_AND_TROW_FORMATTED(L_GLFNI_ERR, err, "Error searching for source %1%", %source);
    }
    if(entry_list.size()) {
        ChaosUniquePtr<chaos::common::data::CDataWrapper> tmp_result(new CDataWrapper());
        for(LogEntryListIterator it = entry_list.begin();
            it != entry_list.end();
            it++){
            tmp_result->appendCDataWrapperToArray(*LogUtility::convertEntry(*(*it).get()));
        }
        tmp_result->finalizeArrayForKey("result_list");
        result = tmp_result.release();
    }
    return result;
}
