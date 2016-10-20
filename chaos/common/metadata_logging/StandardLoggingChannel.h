/*
 *	StandardLoggingChannel.h
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

#ifndef __CHAOSFramework__B658895_85D0_416C_AC0E_BEC5E3A37D0E_StandardLoggingChannel_h
#define __CHAOSFramework__B658895_85D0_416C_AC0E_BEC5E3A37D0E_StandardLoggingChannel_h

#include <chaos/common/exception/exception.h>
#include <chaos/common/metadata_logging/AbstractMetadataLogChannel.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace metadata_logging {
            
#define LOG_DEBUG_DESCRIPTION   "Debug"
#define LOG_INFO_DESCRIPTION    "Info"
#define LOG_WARNING_DESCRIPTION "Warning"
#define LOG_ERROR_DESCRIPTION   "Error"
#define LOG_FATAL_DESCRIPTION   "Fatal"
            
            //macro fror channel instancer
#define METADATA_LOGGING_STANDARD_LOGGING_CHANNEL_INSTANCER \
METADATA_LOGGING_CHANNEL_INSTANCER(StandardLoggingChannel)
            
            //! keep the statisti on how much an error code is used
            struct LogStatistic {
                //!sign the last timestamp(in milliseconds) that the log code has been fired
                uint64_t next_fire_ts;
                //!is the delay in millisecond that the code can be processed
                uint64_t delay_for_next_message;
                //!sign the repetition
                uint32_t repetition;
                
                LogStatistic();
                ~LogStatistic();
                
                void prepareForNextFire(uint64_t cur_ts);
            };
            
            //!define the map that will contains the statisti on the fire of a log level-code
            typedef std::pair<int32_t, int32_t> LogStatisticKey;
            CHAOS_DEFINE_MAP_FOR_TYPE(LogStatisticKey,
                                      LogStatistic,
                                      LogMapStatistic);
            
            class StandardLoggingChannel:
            public AbstractMetadataLogChannel {
                
                typedef enum LogLevel {
                    LogLevelDebug = 0,
                    LogLevelInfo,
                    LogLevelWarning,
                    LogLevelError,
                    LogLevelFatal
                } LogLevel;
                
                LogLevel current_logging_level;
                LogMapStatistic map_log_statistic;
                inline bool loggable(LogLevel log_level);
                
            public:
                StandardLoggingChannel();
                ~StandardLoggingChannel();
                
                //set the log level
                void setLogLevel(LogLevel new_log_level);
                
                void setFireOffsetForLogLevelAndCode(LogLevel log_level,
                                                     int code,
                                                     uint64_t delay);
                //!log an info message
                int logMessage(const std::string& node_uid,
                               const std::string& log_subject,
                               const LogLevel log_level,
                               const int32_t code,
                               const std::string& message,
                               const std::string& domain);
                
            };
            
        }
    }
}

#endif /* __CHAOSFramework__B658895_85D0_416C_AC0E_BEC5E3A37D0E_StandardLoggingChannel_h */
