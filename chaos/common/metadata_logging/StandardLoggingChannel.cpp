/*
 *	StandardLoggingChannel.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 20/10/2016 INFN, National Institute of Nuclear Physics
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
