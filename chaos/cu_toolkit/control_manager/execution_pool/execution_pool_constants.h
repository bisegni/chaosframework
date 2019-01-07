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

#ifndef __CHAOSFramework__71F52C7_F977_4086_8294_15E4C905A1BB_execution_pool_constants_h
#define __CHAOSFramework__71F52C7_F977_4086_8294_15E4C905A1BB_execution_pool_constants_h
namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace execution_pool {
                
#define CONTROL_MANAGER_EXECUTION_POOLS                 "execution_pools"
#define CONTROL_MANAGER_EXECUTION_POOLS_DESC            "Is the list of execution pool to use for request job"
                
#define CONTROL_MANAGER_EXECUTION_POOLS_CHECK_TIME      30000
                
#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP         "execution_pools_cpu_cap"
#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DEFAULT 70
#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DESC    "Is the maximum percentage of cpu occupancy, used to check when unit server don't must ask anymore for new job"
                
            }
        }
    }
}
#endif /* __CHAOSFramework__71F52C7_F977_4086_8294_15E4C905A1BB_execution_pool_constants_h */
