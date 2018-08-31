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

#include "SubmitEntry.h"

#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::metadata_service::api::logging;

#define L_SE_INFO INFO_LOG(SubmitEntry)
#define L_SE_DBG  DBG_LOG(SubmitEntry)
#define L_SE_ERR  ERR_LOG(SubmitEntry)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::metadata_service::api::logging;
using namespace chaos::metadata_service::persistence::data_access;

SubmitEntry::SubmitEntry():
AbstractApi(MetadataServerLoggingDefinitionKeyRPC::ACTION_NODE_LOGGING_SUBMIT_ENTRY){
    alert_event_channel = NetworkBroker::getInstance()->getNewAlertEventChannel();
}

SubmitEntry::~SubmitEntry() {
    if(alert_event_channel) {NetworkBroker::getInstance()->disposeEventChannel(alert_event_channel);}
}

CDWUniquePtr SubmitEntry::execute(CDWUniquePtr api_data) {
    int err = 0;
    
    //check for mandatory attributes
    CHECK_CDW_THROW_AND_LOG(api_data, L_SE_ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, L_SE_ERR, -2, "The log timestamp key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER), L_SE_ERR, -3, "The log timestamp key needs to be a string value");
    CHECK_KEY_THROW_AND_LOG(api_data, MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP, L_SE_ERR, -4, "The log timestamp key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isInt64Value(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP), L_SE_ERR, -5, "The log timestamp key needs to be an int64 value");
    CHECK_KEY_THROW_AND_LOG(api_data, MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN, L_SE_ERR, -6, "The log domain key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN), L_SE_ERR, -7, "The log domain needs to be a string");
    CHECK_KEY_THROW_AND_LOG(api_data, MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT, L_SE_ERR, -7, "The log subject key is mandatory");
    CHAOS_LASSERT_EXCEPTION(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT), L_SE_ERR, -7, "The log subject needs to be a string");
    GET_DATA_ACCESS(LoggingDataAccess, l_da, -8);
    
    //crete entry
    LogEntry new_log_entry;
    new_log_entry.source_identifier = api_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER);
    new_log_entry.ts = api_data->getUInt64Value(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP);
    new_log_entry.domain = api_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN);
    new_log_entry.subject = api_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT);
    //compelte log antry with log channel custom key
    completeLogEntry(api_data,
                     new_log_entry);
    
    //insert the log entry
    if((err = l_da->insertNewEntry(new_log_entry))){
        LOG_AND_TROW(L_SE_ERR, -9, "Error creaating new log entry");
    }
    
    //we can send the event
    if(alert_event_channel){
        //send broadcast event
        alert_event_channel->sendLogAlert(new_log_entry.source_identifier,
                                          new_log_entry.domain);
    }
    return NULL;
}

void SubmitEntry::completeLogEntry(CDataWrapper *api_data,
                                   LogEntry& new_log_entry) {
    
    //compose entry
    std::vector<std::string> all_keys_in_pack;
    api_data->getAllKey(all_keys_in_pack);
    for(std::vector<std::string>::iterator it = all_keys_in_pack.begin();
        it != all_keys_in_pack.end();
        it++) {
        if(it->compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER) == 0 ||
           it->compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP) == 0 ||
           it->compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN) == 0 ||
           it->compare(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT) == 0) continue;
        
        if(api_data->isBoolValue(*it)) {
            new_log_entry.map_bool_value.insert(make_pair(*it,api_data->getBoolValue(*it)));
        } else if(api_data->isInt32Value(*it)) {
            new_log_entry.map_int32_value.insert(make_pair(*it,api_data->getInt32Value(*it)));
        } else if(api_data->isInt64Value(*it)) {
            new_log_entry.map_int64_value.insert(make_pair(*it,api_data->getInt64Value(*it)));
        } else if(api_data->isDoubleValue(*it)) {
            new_log_entry.map_double_value.insert(make_pair(*it,api_data->getDoubleValue(*it)));
        } else if(api_data->isStringValue(*it)) {
            new_log_entry.map_string_value.insert(make_pair(*it,api_data->getStringValue(*it)));
        }
    }
}
