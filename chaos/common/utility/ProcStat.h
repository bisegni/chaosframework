/*
 *	ProcStat.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 14/12/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__C1FF86D_40E3_45CC_9BAD_551F0A5629B7_ProcStat_h
#define __CHAOSFramework__C1FF86D_40E3_45CC_9BAD_551F0A5629B7_ProcStat_h

#include <stdint.h>

#include <sys/resource.h>

namespace chaos {
    namespace common {
        namespace utility {
            
            class ProcStatCalculator;
            
            struct ProcStat {
                friend class ProcStatCalculator;
                double usr_time;
                double sys_time;
                double swap_rsrc;
                uint64_t uptime;
                ProcStat();
                ProcStat& operator=(ProcStat const &rhs);
            private:
                //last sampling resurce usage
                double last_usr_time;
                double last_sys_time;
                double last_total_time;
                uint64_t last_sampling_time;
                uint64_t creation_time;
            };
            
            class ProcStatCalculator {
            public:
                static void update(ProcStat& stat);
            };
        }
    }
}

#endif /* __CHAOSFramework__C1FF86D_40E3_45CC_9BAD_551F0A5629B7_ProcStat_h */
