/*
 *	ErrorLoggingChannel.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 10/02/16 INFN, National Institute of Nuclear Physics
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
