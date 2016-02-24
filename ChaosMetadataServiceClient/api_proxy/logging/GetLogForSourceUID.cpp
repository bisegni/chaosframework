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

#include <ChaosMetadataServiceClient/api_proxy/logging/GetLogForSourceUID.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::logging;

API_PROXY_CD_DEFINITION(GetLogForSourceUID,
                        MetadataServerLoggingDefinitionKeyRPC::ACTION_NODE_LOGGING_RPC_DOMAIN,
                        "getLogForNodeUID")
/*!
 
 */
ApiProxyResult GetLogForSourceUID::execute(const std::string& source,
                                           const std::string& domain,
                                           const uint64_t last_sequence_id,
                                           const uint32_t page_length) {
    std::auto_ptr<CDataWrapper> pack(new CDataWrapper());
    pack->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, source);
    if(last_sequence_id) {pack->addInt64Value("seq", last_sequence_id);}
    if(domain.size()) {pack->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN, domain);}
    pack->addInt32Value("page_length", page_length);
    return callApi(pack.release());
}

ApiProxyResult GetLogForSourceUID::execute(const std::string& source,
                                           const LogDomainList& domain_list,
                                           const uint64_t last_sequence_id,
                                           const uint32_t page_length) {
    std::auto_ptr<CDataWrapper> pack(new CDataWrapper());
    pack->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, source);
    if(last_sequence_id ) {pack->addInt64Value("seq", last_sequence_id);}
    if(domain_list.size()) {
        for(LogDomainListConstIterator it= domain_list.begin();
            it!= domain_list.end();
            it++) {
            pack->appendStringToArray(*it);
        }
        pack->finalizeArrayForKey(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN);
    }
    pack->addInt32Value("page_length", page_length);
    return callApi(pack.release());
}

std::auto_ptr<GetLogForSourceUIDHelper> GetLogForSourceUID::getHelper(CDataWrapper *api_result) {
    return std::auto_ptr<GetLogForSourceUIDHelper>(new GetLogForSourceUIDHelper(api_result));
}

GetLogForSourceUIDHelper::GetLogForSourceUIDHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("result_list")) return;
    
    std::vector<std::string> contained_key;
    std::auto_ptr<CMultiTypeDataArrayWrapper> vec(api_result->getVectorValue("result_list"));
    for(int idx = 0;
        idx < vec->size();
        idx++) {
        std::auto_ptr<CDataWrapper> entry_dw(vec->getCDataWrapperElementAtIndex(idx));
        
        boost::shared_ptr<LogEntry> entry(new LogEntry());
        
        contained_key.clear();
        entry_dw->getAllKey(contained_key);
        
        for(std::vector<std::string>::iterator it =  contained_key.begin();
            it != contained_key.end();
            it++) {
            if(it->compare("seq") == 0) {
                entry->sequence = entry_dw->getInt32Value("seq");
            }else if(it->compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER) == 0) {
                entry->source_identifier = entry_dw->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER);
            } else if(it->compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP) == 0){
                entry->ts = entry_dw->getUInt64Value(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP);
            } else if(it->compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)==0){
                entry->domain = entry_dw->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN);
            } else if(it->compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT)==0){
                entry->subject = entry_dw->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT);
            } else {
                switch(entry_dw->getValueType(*it)) {
                    case CDataWrapperTypeNoType:
                        break;
                    case CDataWrapperTypeNULL:
                        break;
                    case CDataWrapperTypeBool:
                        entry->map_bool_value.insert(make_pair(*it, entry_dw->getInt32Value(*it)));
                        break;
                    case CDataWrapperTypeInt32:
                        entry->map_int32_value.insert(make_pair(*it, entry_dw->getInt32Value(*it)));
                        break;
                    case CDataWrapperTypeInt64:
                        entry->map_int64_value.insert(make_pair(*it, entry_dw->getInt64Value(*it)));
                        break;
                    case CDataWrapperTypeDouble:
                        entry->map_double_value.insert(make_pair(*it, entry_dw->getDoubleValue(*it)));
                        break;
                    case CDataWrapperTypeString:
                        entry->map_string_value.insert(make_pair(*it, entry_dw->getStringValue(*it)));
                        break;
                    case CDataWrapperTypeBinary:
                        break;
                    case CDataWrapperTypeObject:
                        break;
                    case CDataWrapperTypeVector:
                        break;
                }
            }
        }
        log_entry_list.push_back(entry);
    }
}

GetLogForSourceUIDHelper::~GetLogForSourceUIDHelper() {}

size_t GetLogForSourceUIDHelper::getLogEntryListSize() {
    return log_entry_list.size();
}

const LogEntryList& GetLogForSourceUIDHelper::getLogEntryList(){
    return log_entry_list;
}