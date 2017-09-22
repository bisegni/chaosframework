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
