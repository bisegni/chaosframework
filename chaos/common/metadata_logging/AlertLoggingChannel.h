/*
 *	AlertLoggingChannel.h
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

#ifndef __CHAOSFramework_C3D81B8D_2073_44DC_85E9_C94F418D5DE9_AllertLoggingChannel_h
#define __CHAOSFramework_C3D81B8D_2073_44DC_85E9_C94F418D5DE9_AllertLoggingChannel_h


#include <chaos/common/exception/exception.h>
#include <chaos/common/metadata_logging/AbstractMetadataLogChannel.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace metadata_logging {
            
#define ALERT_CLEAR_DESCRIPTION     "Clear"
#define ALERT_LOW_DESCRIPTION       "Low"
#define ALERT_ELEVATED_DESCRIPTION  "Elevated"
#define ALERT_HIGH_DESCRIPTION      "High"
#define ALERT_SEVERE_DESCRIPTION    "Severe"
            
            //macro fror channel instancer
#define METADATA_LOGGING_ALERT_LOGGING_CHANNEL_INSTANCER \
METADATA_LOGGING_CHANNEL_INSTANCER(AlertLoggingChannel)
            //!alert level
            typedef enum AlertLevel {
                AlertLevelClear = 0,//green
                AlertLevelLow,//blue
                AlertLevelElevated,//yellow
                AlertLevelHigh,//orange
                AlertLevelSevere//red
            } LogLevel;

            
            class AlertLoggingChannel:
            public AbstractMetadataLogChannel {
                
                //! keep the statisti on how much an error code is used
                struct AlertDescription {
                    //!alert code
                    const int alert_code;
                    //!alert description
                    const std::string alert_description;
                    //!sign the last timestamp(in milliseconds) that the log code has been fired
                    uint64_t next_fire_ts;
                    //!is the delay in millisecond that the code can be processed
                    uint64_t delay_for_next_message;
                    //!sign the repetition
                    uint32_t repetition;
                    //!last alert level
                    AlertLevel last_level;
                    
                    AlertDescription(const int _code,
                                     const std::string& _description);
                    ~AlertDescription();
                    
                    bool setLevel(AlertLevel current_level);
                };
                
                //!define the map that will contains the statisti on the fire of a log level-code
                typedef int32_t AlertDescriptionKey;
                CHAOS_DEFINE_MAP_FOR_TYPE(AlertDescriptionKey,
                                          boost::shared_ptr<AlertDescription>,
                                          MapAlertDescription);
                
                MapAlertDescription map_alert_description;
            public:
                AlertLoggingChannel();
                ~AlertLoggingChannel();
                
                //!add new alert
                bool addNewAlert(int alert_code,
                                 const std::string& alert_description);
                //! set a new delay for alert publication
                /*!
                 set a new delay for the alert publication, if alert is sent
                 many time within this delay, the number of time it is occured
                 is memorized into the repetition field of AlertDescription structure
                 for that alert
                 \param alert_code the alert that needs to be modifed
                 \param delay_in_ms the daly expressed in milliseconds
                 */
                bool setAlertPublishDelay(int alert_code,
                                          const uint64_t delay_in_ms);
                
                //!log an info message
                int logAlert(const std::string& node_uid,
                             const std::string& log_subject,
                             const int32_t alert_code,
                             const AlertLevel alert_level);
            };
            
        }
    }
}



#endif /* __CHAOSFramework_C3D81B8D_2073_44DC_85E9_C94F418D5DE9_AllertLoggingChannel_h */
