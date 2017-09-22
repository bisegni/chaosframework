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
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_TAG, alarm_description.getAlarmTag());
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_NAME, alarm_description.getAlarmName());
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_LEVEL_CODE, alarm_description.getCurrentSeverityCode());
    //log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_NAME, alarm_description.alarm_name);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_DESCRIPTION, alarm_description.getAlarmDescription());
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlarmLogging::PARAM_NODE_LOGGING_ALARM_LEVEL_DESCRIPTION, alarm_description.getCurrentSeverityDescription());
    return sendLog(log_entry,
                   0);
}
