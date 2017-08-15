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
#ifndef __CHAOSFramework__ProcInfo_h
#define __CHAOSFramework__ProcInfo_h

#include <sigar.h>

namespace chaos {
    namespace common {
        namespace debug {
            class ProcInfo {
                sigar_t *s;
                sigar_pid_t my_pid;
                sigar_proc_mem_t my_proc_info;
                ProcInfo();
                ~ProcInfo();
                int getProcInfo();
            };
        }
    }
}

#endif /* ProcInfo_hpp */
