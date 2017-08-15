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
