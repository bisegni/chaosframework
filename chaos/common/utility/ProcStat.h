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
