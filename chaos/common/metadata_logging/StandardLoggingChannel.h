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
