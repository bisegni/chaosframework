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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/metadata_logging/StandardLoggingChannel.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::metadata_logging;

#pragma mark StandardLoggingChannel
StandardLoggingChannel::StandardLoggingChannel():
AbstractMetadataLogChannel(){}

StandardLoggingChannel::~StandardLoggingChannel() {}

bool StandardLoggingChannel::loggable(LogLevel log_level) {
    return ((int)log_level) >= current_logging_level;
}

void StandardLoggingChannel::setLogLevel(LogLevel new_log_level) {
    current_logging_level = new_log_level;
}

int StandardLoggingChannel::logMessage(const std::string& node_uid,
                                       const std::string& log_subject,
                                       const LogLevel log_level,
                                       const std::string& message) {
    if(loggable(log_level) == false) return 0;
    
    std::string log_description;
    CDataWrapper *log_entry = getNewLogEntry(node_uid,
                                             log_subject,
                                             "log");
    switch (log_level) {
        case LogLevelDebug:
            log_description = LOG_DEBUG_DESCRIPTION;
            break;
        case LogLevelInfo:
            log_description = LOG_INFO_DESCRIPTION;
            break;
        case LogLevelWarning:
            log_description = LOG_WARNING_DESCRIPTION;
            break;
        case LogLevelError:
            log_description = LOG_ERROR_DESCRIPTION;
            break;
        case LogLevelFatal:
            log_description = LOG_FATAL_DESCRIPTION;
            break;
    }
    
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::StandardLogging::PARAM_NODE_LOGGING_LOG_LEVEL_CODE, log_level);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::StandardLogging::PARAM_NODE_LOGGING_LOG_LEVEL_DESCRIPTION, log_description);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::StandardLogging::PARAM_NODE_LOGGING_LOG_MESSAGE, message);
    return sendLog(log_entry,
                   0);
}
