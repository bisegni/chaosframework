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
                                    chaos::common::data::CDataWrapper *message_data);
                
                int logCommandRunningProperty(const std::string& log_emitter,
                                              const std::string& log_subject,
                                              const uint64_t command_id,
                                              const uint8_t rprop);

            };
            
        }
    }
}

#endif /* __CHAOSFramework__BatchCommandLoggingChannel_h */
