/*
 *	BatchCommandLoggingChannel.h
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

#ifndef __CHAOSFramework__BatchCommandLoggingChannel_h
#define __CHAOSFramework__BatchCommandLoggingChannel_h

#include <chaos/common/batch_command/batch_command.h>
#include <chaos/common/metadata_logging/AbstractMetadataLogChannel.h>

namespace chaos {
    namespace common {
        namespace metadata_logging {
            
            //macro fror channel instancer
#define METADATA_LOGGING_COMMAND_LOGGING_CHANNEL_INSTANCER \
METADATA_LOGGING_CHANNEL_INSTANCER(ErrorLoggingChannel)
            
            class BatchCommandLoggingChannel:
            public AbstractMetadataLogChannel {
            public:
                BatchCommandLoggingChannel();
                ~BatchCommandLoggingChannel();
                
                //!log a detailed error on metadata server
                int logCommandState(const std::string& log_emitter,
                                    const std::string& log_subject,
                                    const uint64_t command_id,
                                    chaos::common::batch_command::BatchCommandEventType::BatchCommandEventType command_event,
                                    CDataWrapper *message_data);
                
                int logCommandRunningProperty(const std::string& log_emitter,
                                              const std::string& log_subject,
                                              const uint64_t command_id,
                                              const uint8_t rprop);

            };
            
        }
    }
}

#endif /* __CHAOSFramework__BatchCommandLoggingChannel_h */
