/*
 *	AllertLoggingChannel.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 21/10/2016 INFN, National Institute of Nuclear Physics
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
#include <chaos/common/metadata_logging/AlarmLoggingChannel.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::alarm;
using namespace chaos::common::utility;
using namespace chaos::common::metadata_logging;

#pragma mark StandardLoggingChannel
AlarmLoggingChannel::AlarmLoggingChannel():
AbstractMetadataLogChannel(){}

AlarmLoggingChannel::~AlarmLoggingChannel() {}

int AlarmLoggingChannel::logAlarm(const std::string& node_uid,
                                  const std::string& log_subject,
                                  const chaos::common::alarm::AlarmDescription& alarm_description) {
    CDataWrapper *log_entry = getNewLogEntry(node_uid,
                                             log_subject,
                                             "alarm");
    
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_CODE, alarm_description.alarm_code);
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_LEVEL_CODE, alarm_description.current_level);
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_REPETITION, alarm_description.stat.repetition);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_NAME, alarm_description.alarm_name);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_DESCRIPTION, alarm_description.alarm_description);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_LEVEL_DESCRIPTION, AlarmDescription::decodeAlarmLevelDescription(alarm_description));
    return sendLog(log_entry,
                   0);
}
