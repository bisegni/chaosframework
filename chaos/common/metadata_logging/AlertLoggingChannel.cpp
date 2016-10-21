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
#include <chaos/common/metadata_logging/AlertLoggingChannel.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::metadata_logging;

#pragma mark LogStatistic
AlertLoggingChannel::AlertDescription::AlertDescription(const int _code,
                                                        const std::string& _description):
alert_code(_code),
alert_description(_description),
next_fire_ts(0),
delay_for_next_message(0),
repetition(0),
last_level(AlertLevelClear){}

AlertLoggingChannel::AlertDescription::~AlertDescription() {}

bool AlertLoggingChannel::AlertDescription::setLevel(AlertLevel current_level) {
    bool publisheable = false;
    //check the statistic to check if we can fire the log
    uint64_t now = TimingUtil::getTimeStamp();
    
    publisheable =  current_level != last_level ||
    now>=next_fire_ts;
    if(publisheable) {
        repetition = 0;
        last_level = current_level;
        next_fire_ts = now+delay_for_next_message;
    }
    repetition++;
    return publisheable;
}

#pragma mark StandardLoggingChannel
AlertLoggingChannel::AlertLoggingChannel():
AbstractMetadataLogChannel(){}

AlertLoggingChannel::~AlertLoggingChannel() {}

bool AlertLoggingChannel::addNewAlert(int alert_code,
                                      const std::string& alert_description) {
    //check if alert code is already present
    MapAlertDescriptionIterator found = map_alert_description.find(alert_code);
    if(found != map_alert_description.end()) return false;
    
    //we can insert new alert code
    map_alert_description.insert(MapAlertDescriptionPair(alert_code,
                                                         boost::shared_ptr<AlertDescription>(new AlertDescription(alert_code,
                                                                                                                  alert_description))));
    return true;
}

bool AlertLoggingChannel::setAlertPublishDelay(int alert_code,
                                               const uint64_t delay_in_ms) {
    //check if alert code is already present
    MapAlertDescriptionIterator found = map_alert_description.find(alert_code);
    if(found != map_alert_description.end()) return false;
    found->second->delay_for_next_message = delay_in_ms;
    
    return true;
}

int AlertLoggingChannel::logAlert(const std::string& node_uid,
                                  const std::string& log_subject,
                                  const int32_t alert_code,
                                  const AlertLevel alert_level) {
    //check if we have alert
    MapAlertDescriptionIterator found = map_alert_description.find(alert_code);
    if(found == map_alert_description.end()) return -1;
    
    if(found->second->setLevel(alert_level) == false) return 0;
    
    std::string level_description;
    CDataWrapper *log_entry = getNewLogEntry(node_uid,
                                             log_subject,
                                             "alert");
    switch (alert_level) {
        case AlertLevelClear:
            level_description = ALERT_CLEAR_DESCRIPTION;
            break;
        case AlertLevelLow:
            level_description = ALERT_LOW_DESCRIPTION;
            break;
        case AlertLevelElevated:
            level_description = ALERT_ELEVATED_DESCRIPTION;
            break;
        case AlertLevelHigh:
            level_description = ALERT_HIGH_DESCRIPTION;
            break;
        case AlertLevelSevere:
            level_description = ALERT_SEVERE_DESCRIPTION;
            break;
    }
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::AlertLogging::PARAM_NODE_LOGGING_ALERT_CODE, found->second->alert_code);
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::AlertLogging::PARAM_NODE_LOGGING_ALERT_LEVEL_CODE, found->second->last_level);
    log_entry->addInt32Value(MetadataServerLoggingDefinitionKeyRPC::AlertLogging::PARAM_NODE_LOGGING_ALERT_REPETITION, found->second->repetition);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlertLogging::PARAM_NODE_LOGGING_ALERT_DESCRIPTION, found->second->alert_description);
    log_entry->addStringValue(MetadataServerLoggingDefinitionKeyRPC::AlertLogging::PARAM_NODE_LOGGING_ALERT_LEVEL_DESCRIPTION, level_description);
    return sendLog(log_entry,
                   0);
}
