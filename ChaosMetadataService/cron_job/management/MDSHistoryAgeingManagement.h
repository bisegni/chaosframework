/*
 *	MDSHistoryAgeingManagement.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/09/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_E5E3B772_B719_4BCE_9EEB_920A02DFB3DD_MDSHistoryAgeingManagement_h
#define __CHAOSFramework_E5E3B772_B719_4BCE_9EEB_920A02DFB3DD_MDSHistoryAgeingManagement_h

#include "../MDSCronJob.h"

namespace chaos {
    namespace metadata_service {
        namespace cron_job {
            
            class MDSHistoryAgeingManagement:
            public MDSCronJob {
                uint64_t last_sequence_found;
                void start();
                bool execute(const common::cronous_manager::MapKeyVariant& job_parameter);
                void end();
            public:
                MDSHistoryAgeingManagement(chaos::common::data::CDataWrapper *param);
                ~MDSHistoryAgeingManagement();
            };
            
        }
    }
}

#endif /* __CHAOSFramework_E5E3B772_B719_4BCE_9EEB_920A02DFB3DD_MDSHistoryAgeingManagement_h */
