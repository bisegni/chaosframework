/*
 *	MetadataLoggingManager.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 02/02/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__MetadataLoggingManager_h
#define __CHAOSFramework__MetadataLoggingManager_h

#include <chaos/common/utility/Singleton.h>

namespace chaos {
    namespace common {
        namespace metadata_logging {
            
            //! Metadata Logging Service
            /*!
             The metadata logging service, permit to store into !CHAOS MDS, metadata information.
             They can be, error, command or other thing that nodes need to be store on MDS.
             */
            class MetadataLoggingManager:
            public chaos::common::utility::Singleton<MetadataLoggingManager> {
                friend class chaos::common::utility::Singleton<MetadataLoggingManager>;
            };
            
        }
    }
}

#endif /* __CHAOSFramework__MetadataLoggingManager_h */
