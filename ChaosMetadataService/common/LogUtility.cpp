/*
 *	LogUtility.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 25/02/16 INFN, National Institute of Nuclear Physics
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

#include "LogUtility.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::persistence::data_access;
#define WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(o, m, miter, map)\
for(miter it = map.begin();\
it != map.end();\
it++) {\
o->m(it->first, it->second);\
}

std::auto_ptr<CDataWrapper> LogUtility::convertEntry(LogEntry& log_entry) {
    std::auto_ptr<CDataWrapper> result(new CDataWrapper());
    result->addInt64Value("seq", log_entry.sequence);
    result->addInt64Value(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_TIMESTAMP, log_entry.ts);
    result->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SOURCE_IDENTIFIER, log_entry.source_identifier);
    result->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN, log_entry.domain);
    result->addStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_SUBJECT, log_entry.subject);
    
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addStringValue, LoggingKeyValueStringMapIterator, log_entry.map_string_value);
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addInt64Value, LoggingKeyValueInt64MapIterator, log_entry.map_int64_value);
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addInt32Value, LoggingKeyValueInt32MapIterator, log_entry.map_int32_value);
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addDoubleValue, LoggingKeyValueDoubleMapIterator, log_entry.map_double_value);
    WRITE_LOG_ATTRIBUTE_MAP_ON_OBJECT(result, addBoolValue, LoggingKeyValueBoolMapIterator, log_entry.map_bool_value);
    return result;
}