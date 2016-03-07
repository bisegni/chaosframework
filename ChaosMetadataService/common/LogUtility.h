/*
 *	LogUtility.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 25/02/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__LogUtility_h
#define __CHAOSFramework__LogUtility_h

#include "../persistence/persistence.h"

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service {
        namespace common {
            
            class LogUtility {
            public:
                static std::auto_ptr<chaos::common::data::CDataWrapper> convertEntry(persistence::data_access::LogEntry& log_entry);
            };
            
        }
    }
}



#endif /* __CHAOSFramework__LogUtility_h */