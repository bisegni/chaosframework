/*
 *	GetLogForSourceUID.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 15/02/16 INFN, National Institute of Nuclear Physics
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

#include "GetLogForSourceUID.h"

#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::metadata_service::api::logging;

#define L_GLFNI_INFO INFO_LOG(GetLogForSourceUID)
#define L_GLFNI_DBG  DBG_LOG(GetLogForSourceUID)
#define L_GLFNI_ERR  ERR_LOG(GetLogForSourceUID)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::metadata_service::api::logging;
using namespace chaos::metadata_service::persistence::data_access;

GetLogForSourceUID::GetLogForSourceUID():
AbstractApi("getLogForNodeUID"){
}

GetLogForSourceUID::~GetLogForSourceUID() {
}

chaos::common::data::CDataWrapper *GetLogForSourceUID::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    
    CDataWrapper *result = NULL;

    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, L_GLFNI_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, L_GLFNI_ERR, -2, "The log timestamp key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER), L_GLFNI_ERR, -3, "The log timestamp key needs to be a string value");
    
    GET_DATA_ACCESS(LoggingDataAccess, l_da, -4);
    
    //entry list
    LogEntryList entry_list;
    std::vector<std::string> domain_to_include;
    
    bool page_direction = api_data->getValueWithDefault<bool>("page_direction",true);
    uint32_t page_length =  (uint32_t)api_data->getValueWithDefault<int32_t>("page_length",100);
    uint64_t sequence = (uint64_t)api_data->getValueWithDefault<int64_t>("seq", 0);
    if(api_data->hasKey(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
        //we have domain
        if(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
            domain_to_include.push_back(api_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN));
        } else if(api_data->isVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
            std::auto_ptr<CMultiTypeDataArrayWrapper> domain_vec(api_data->getVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN));
            for(int idx = 0;
                idx < domain_vec->size();
                idx++){
                domain_to_include.push_back(domain_vec->getStringElementAtIndex(idx));
            }
        } else {
            LOG_AND_TROW_FORMATTED(L_GLFNI_ERR, -5, "Domain key '%1% 'need to be string or array of string",%MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN);
        }
        
    }
    const std::string source = api_data->getValueWithDefault<std::string>(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, "");
    
    if((err = l_da->searchEntryForSource(entry_list,
                                         source,
                                         domain_to_include,
                                         sequence,
                                         page_length,
                                         page_direction))) {
        LOG_AND_TROW_FORMATTED(L_GLFNI_ERR, err, "Error searching for source %1%", %source);
    }
    if(entry_list.size()) {
        std::auto_ptr<CDataWrapper> tmp_result(new CDataWrapper());
        for(LogEntryListIterator it = entry_list.begin();
            it != entry_list.end();
            it++){
            tmp_result->appendCDataWrapperToArray(*converEntry(*(*it).get()));
        }
        tmp_result->finalizeArrayForKey("result_list");
        result = tmp_result.release();
    }
    return result;
}

#define WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(o, m, miter, map)\
for(miter it = map.begin();\
it != map.end();\
it++) {\
o->m(it->first, it->second);\
}

std::auto_ptr<CDataWrapper> GetLogForSourceUID::converEntry(LogEntry& log_entry) {
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    result->addInt64Value("seq", log_entry.sequence);
    result->addInt64Value(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP, log_entry.ts);
    result->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, log_entry.source_identifier);
    result->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN, log_entry.domain);
    
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addStringValue, LoggingKeyValueStringMapIterator, log_entry.map_string_value);
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addInt64Value, LoggingKeyValueInt64MapIterator, log_entry.map_int64_value);
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addInt32Value, LoggingKeyValueInt32MapIterator, log_entry.map_int32_value);
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addDoubleValue, LoggingKeyValueDoubleMapIterator, log_entry.map_double_value);
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addBoolValue, LoggingKeyValueBoolMapIterator, log_entry.map_bool_value);
    
    return result;
}