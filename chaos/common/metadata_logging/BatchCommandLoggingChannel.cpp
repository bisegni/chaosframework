/*
 *	BatchCommandLoggingChannel.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 23/02/16 INFN, National Institute of Nuclear Physics
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


#include <chaos/common/metadata_logging/BatchCommandLoggingChannel.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::metadata_logging;
using namespace chaos::common::batch_command::BatchCommandEventType;

BatchCommandLoggingChannel::BatchCommandLoggingChannel():
AbstractMetadataLogChannel(){
    
}

BatchCommandLoggingChannel::~BatchCommandLoggingChannel() {
    
}

int BatchCommandLoggingChannel::logCommandState(const std::string& node_uid,
                                                const uint64_t command_id,
                                                BatchCommandEventType command_event) {
    CDataWrapper *log_entry = getNewLogEntry(node_uid, "command");
    log_entry->addInt64Value(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_ID, command_id);
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_STATE, command_event);
    switch(command_event) {
        case EVT_QUEUED:
            log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_STATE_DESCRIPTION, "Queued");
            break;
        case EVT_WAITING:
            log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_STATE_DESCRIPTION, "Waiting");
            break;
        case EVT_RUNNING:
            log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_STATE_DESCRIPTION, "Running");
            break;
        case EVT_PAUSED:
            log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_STATE_DESCRIPTION, "Paused");
            break;
        case EVT_COMPLETED:
            log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_STATE_DESCRIPTION, "Complete");
            break;
        case EVT_FAULT:
            log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_STATE_DESCRIPTION, "Fault");
            break;
        case EVT_KILLED:
            log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::CommandLogging::PARAM_NODE_LOGGING_LOG_COMMAND_STATE_DESCRIPTION, "Killed");
            break;
    }

    return sendLog(log_entry,
                   true);
}

