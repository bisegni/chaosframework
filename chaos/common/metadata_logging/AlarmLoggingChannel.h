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

#include <chaos/common/alarm/AlarmDescription.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/metadata_logging/AbstractMetadataLogChannel.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace common {
        namespace metadata_logging {
            //macro fror channel instancer
#define METADATA_LOGGING_ALARM_LOGGING_CHANNEL_INSTANCER \
METADATA_LOGGING_CHANNEL_INSTANCER(AlarmLoggingChannel)
            
            class AlarmLoggingChannel:
            public AbstractMetadataLogChannel {
                
            public:
                AlarmLoggingChannel();
                ~AlarmLoggingChannel();

                //!log an info message
                int logAlarm(const std::string& node_uid,
                             const std::string& log_subject,
                             const chaos::common::alarm::AlarmDescription& alarm_description);
            };
        }
    }
}



#endif /* __CHAOSFramework_C3D81B8D_2073_44DC_85E9_C94F418D5DE9_AllertLoggingChannel_h */
