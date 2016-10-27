/*
 *	TestStateFlagCatalog.h
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by bisegni.
 *
 *    	Copyright 22/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFrameworkTests_D40485EA_AE3A_461F_818A_4272F4DC9097_TestStateFlagCatalog_h
#define __CHAOSFrameworkTests_D40485EA_AE3A_461F_818A_4272F4DC9097_TestStateFlagCatalog_h
#include <chaos/common/alarm/AlarmCatalog.h>
#include <chaos/common/state_flag/StateFlagCatalog.h>

namespace chaos {
    namespace common {
        namespace utility {
            namespace test {
                
                class TestStateFlagCatalog {
                    chaos::common::alarm::AlarmCatalog alarm_catalog;
                    chaos::common::state_flag::StateFlagCatalog catalog_a;
                    chaos::common::state_flag::StateFlagCatalog catalog_b;
                public:
                    TestStateFlagCatalog();
                    ~TestStateFlagCatalog();
                    bool test();
                };
            }
        }
    }
}

#endif /* __CHAOSFrameworkTests_D40485EA_AE3A_461F_818A_4272F4DC9097_TestStateFlagCatalog_h */
