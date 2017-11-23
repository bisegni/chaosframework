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

#include "LoggingDataAccess.h"
#include <chaos/common/global.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::metadata_service::persistence::data_access;

#define INFO INFO_LOG(LoggingDataAccess)
#define DBG  DBG_LOG(LoggingDataAccess)
#define ERR  ERR_LOG(LoggingDataAccess)

DEFINE_DA_NAME(LoggingDataAccess)

//! default constructor
LoggingDataAccess::LoggingDataAccess():
AbstractDataAccess("LoggingDataAccess"){}

//!default destructor
LoggingDataAccess::~LoggingDataAccess(){}

LogEntryUniuePtr LoggingDataAccess::getNewLogEntry(const std::string& log_emitter,
                                                   const std::string& log_subject,
                                                   const std::string& log_domain) {
    LogEntryUniuePtr log_entry(new LogEntry());
    log_entry->source_identifier = log_emitter;
    log_entry->subject = log_subject;
    log_entry->domain = log_domain;
    log_entry->ts = TimingUtil::getTimeStamp();
    return log_entry;
}
int LoggingDataAccess::logException(const std::string& log_emitter,
                                    const std::string& log_subject,
                                    const std::string& log_domain,
                                    const CException& ex) {
    DBG << CHAOS_FORMAT("Logging emitter:%1% subject:%2% domain:%3% -> %4%", %log_emitter%log_subject%log_domain%ex.what());
    LogEntryUniuePtr log_entry = getNewLogEntry(log_emitter,
                                                log_subject,
                                                log_domain);
    if(log_entry.get() == NULL) return -1;
    log_entry->map_int32_value.insert(LoggingKeyValueInt32MapPair(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_CODE, ex.errorCode));
    log_entry->map_string_value.insert(LoggingKeyValueStringMapPair(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_MESSAGE, ex.errorMessage));
    log_entry->map_string_value.insert(LoggingKeyValueStringMapPair(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_DOMAIN, ex.errorDomain));
    return insertNewEntry(*log_entry.get());
}
int LoggingDataAccess::logError(const std::string& log_emitter,
                                const std::string& log_subject,
                                const std::string& log_domain,
                                const int32_t& error_code,
                                const std::string& error_message,
                                const std::string& error_domain) {
    DBG << CHAOS_FORMAT("Logging emitter:%1% subject:%2% domain:%3% -> %4%", %log_emitter%log_subject%log_domain%CHAOS_FORMAT("%1% - %2% - %3%", %error_code%error_message%error_domain));
    LogEntryUniuePtr log_entry = getNewLogEntry(log_emitter,
                                                log_subject,
                                                log_domain);
    if(log_entry.get() == NULL) return -1;
    log_entry->map_int32_value.insert(LoggingKeyValueInt32MapPair(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_CODE, error_code));
    log_entry->map_string_value.insert(LoggingKeyValueStringMapPair(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_MESSAGE, error_message));
    log_entry->map_string_value.insert(LoggingKeyValueStringMapPair(MetadataServerLoggingDefinitionKeyRPC::ErrorLogging::PARAM_NODE_LOGGING_LOG_ERROR_DOMAIN, error_domain));
    return insertNewEntry(*log_entry.get());
}
