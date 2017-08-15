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

#include <chaos/common/metadata_logging/ErrorLoggingChannel.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::metadata_logging;

ErrorLoggingChannel::ErrorLoggingChannel():
AbstractMetadataLogChannel(){
    
}

ErrorLoggingChannel::~ErrorLoggingChannel() {
    
}

int ErrorLoggingChannel::logError(const std::string& node_uid,
                                  const std::string& log_subject,
                                  const chaos::CException& chaos_exception) {
    return logError(node_uid,
                    log_subject,
                    chaos_exception.errorCode,
                    chaos_exception.errorDomain,
                    chaos_exception.errorMessage);
}

int ErrorLoggingChannel::logError(const std::string& node_uid,
                                  const std::string& log_subject,
                                  int32_t error_code,
                                  const std::string& error_message,
                                  const std::string& error_domain) {
    CDataWrapper *log_entry = getNewLogEntry(node_uid,
                                             log_subject,
                                             "error");
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_CODE, error_code);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_MESSAGE, error_message);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_DOMAIN, error_domain);
    return sendLog(log_entry,
                   0);
}
