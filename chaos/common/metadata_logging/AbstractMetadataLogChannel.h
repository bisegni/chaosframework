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

#ifndef __CHAOSFramework__AbstractMetadataLogChannel_h
#define __CHAOSFramework__AbstractMetadataLogChannel_h

#include <string>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace common {
        
#define METADATA_LOGGING_CHANNEL_INSTANCER(_DriverClass_) \
new chaos::common::utility::TypedObjectInstancer< _DriverClass_, chaos::common::metadata_logging::AbstractMetadataLogChannel >()
        
        namespace metadata_logging {
            
            //!forward declaration
            class MetadataLoggingManager;
            
            //!base class for all log channels
            class AbstractMetadataLogChannel {
                friend class MetadataLoggingManager;
                //instance identifier
                const std::string instance_uuid;
                
                //! reference to loggin manager parent
                MetadataLoggingManager *logging_manager;
            protected:
                AbstractMetadataLogChannel();
                virtual ~AbstractMetadataLogChannel();
                
                //!set the mds message channel for this log channel
                void setLoggingManager(MetadataLoggingManager *_mds_channel);
                
                //! send the log to mds server
                int sendLog(chaos::common::data::CDataWrapper *log_message,
                              int32_t priority = 0);
                
                //!create a new empty log entry for a determinate domiain
                chaos::common::data::CDataWrapper *getNewLogEntry(const std::string& log_emitter,
                                                                  const std::string& log_subject,
                                                                  const std::string& log_domain);
            public:
                const std::string & getInstanceUUID();
            };
        }
    }
}

#endif /* __CHAOSFramework__AbstractMetadataLogChannel_h */
