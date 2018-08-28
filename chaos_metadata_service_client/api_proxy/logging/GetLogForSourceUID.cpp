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

#include <chaos_metadata_service_client/api_proxy/logging/GetLogForSourceUID.h>

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
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, source);
    if(last_sequence_id) {pack->addInt64Value("seq", last_sequence_id);}
    if(domain.size()) {pack->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN, domain);}
    pack->addInt32Value("page_length", page_length);
    return callApi(pack);
}

ApiProxyResult GetLogForSourceUID::execute(const std::string& source,
                                           const LogDomainList& domain_list,
                                           const uint64_t last_sequence_id,
                                           const uint32_t page_length) {
    CDWUniquePtr pack(new CDataWrapper());
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
    return callApi(pack);
}

ChaosUniquePtr<GetLogForSourceUIDHelper> GetLogForSourceUID::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<GetLogForSourceUIDHelper>(new GetLogForSourceUIDHelper(api_result));
}

GetLogForSourceUIDHelper::GetLogForSourceUIDHelper(CDataWrapper *api_result) {
    //now we have the result
    //SnapshotInformationPtr
    if(!api_result || !api_result->hasKey("result_list")) return;
    
    std::vector<std::string> contained_key;
    CMultiTypeDataArrayWrapperSPtr vec = api_result->getVectorValue("result_list");
    for(int idx = 0;
        idx < vec->size();
        idx++) {
        CDWUniquePtr entry_dw(vec->getCDataWrapperElementAtIndex(idx));
        
        ChaosSharedPtr<LogEntry> entry(new LogEntry());
        
        contained_key.clear();
        entry_dw->getAllKey(contained_key);
        
        for(std::vector<std::string>::iterator it =  contained_key.begin();
            it != contained_key.end();
            it++) {
            if(it->compare("seq") == 0) {
                entry->sequence = entry_dw->getInt64Value("seq");
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

                    case chaos::DataType::TYPE_BOOLEAN:
                        entry->map_bool_value.insert(make_pair(*it, entry_dw->getInt32Value(*it)));
                        break;
                    case chaos::DataType::TYPE_INT32:
                        entry->map_int32_value.insert(make_pair(*it, entry_dw->getInt32Value(*it)));
                        break;
                    case chaos::DataType::TYPE_INT64:
                        entry->map_int64_value.insert(make_pair(*it, entry_dw->getInt64Value(*it)));
                        break;
                    case chaos::DataType::TYPE_DOUBLE:
                        entry->map_double_value.insert(make_pair(*it, entry_dw->getDoubleValue(*it)));
                        break;
                    case chaos::DataType::TYPE_STRING:
                        entry->map_string_value.insert(make_pair(*it, entry_dw->getStringValue(*it)));
                        break;
                default:
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
