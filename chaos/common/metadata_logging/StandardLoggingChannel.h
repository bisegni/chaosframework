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

            class StandardLoggingChannel:
            public AbstractMetadataLogChannel {
            public:
                typedef enum LogLevel {
                    LogLevelDebug = 0,
                    LogLevelInfo,
                    LogLevelWarning,
                    LogLevelError,
                    LogLevelFatal
                } LogLevel;

                StandardLoggingChannel();
                ~StandardLoggingChannel();
                
                //set the log level
                void setLogLevel(LogLevel new_log_level);

                //!log an info message
                int logMessage(const std::string& node_uid,
                               const std::string& log_subject,
                               const LogLevel log_level,
                               const std::string& message);
            private:
                LogLevel current_logging_level;
                inline bool loggable(LogLevel log_level);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__B658895_85D0_416C_AC0E_BEC5E3A37D0E_StandardLoggingChannel_h */
