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

ChaosUniquePtr<chaos::common::data::CDataWrapper> LogUtility::convertEntry(LogEntry& log_entry) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new CDataWrapper());
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
