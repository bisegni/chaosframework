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

#ifndef __CHAOSFramework__ErrorLoggingChannel_h
#define __CHAOSFramework__ErrorLoggingChannel_h

#include <chaos/common/exception/exception.h>
#include <chaos/common/metadata_logging/AbstractMetadataLogChannel.h>

namespace chaos {
    namespace common {
        namespace metadata_logging {
  
            //macro fror channel instancer
#define METADATA_LOGGING_ERROR_LOGGING_CHANNEL_INSTANCER \
METADATA_LOGGING_CHANNEL_INSTANCER(ErrorLoggingChannel)
            
            class ErrorLoggingChannel:
            public AbstractMetadataLogChannel {
            public:
                ErrorLoggingChannel();
                ~ErrorLoggingChannel();
                
                //!log a detailed error on metadata server
                int logError(const std::string& node_uid,
                             const std::string& log_subject,
                             int32_t error_code,
                             const std::string& error_message,
                             const std::string& error_domain);
                
                //!log a chaos exception on metadata server
                int logError(const std::string& node_uid,
                             const std::string& log_subject,
                             const chaos::CException& chaos_exception);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__ErrorLoggingChannel_h */
