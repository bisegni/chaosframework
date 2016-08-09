/*
 *	StateFlagCatalogTest.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__434FCB3_385A_4FB4_B40E_654E9B0F6D13_StateFlagCatalogTest_h
#define __CHAOSFramework__434FCB3_385A_4FB4_B40E_654E9B0F6D13_StateFlagCatalogTest_h

#include <chaos/common/status_manager/StatusFlagCatalog.h>

namespace chaos {
    namespace common {
        namespace utility {
            namespace test {
                
                class StatusFlagCatalogTest {
                    chaos::common::status_manager::StatusFlagCatalog catalog_a;
                    chaos::common::status_manager::StatusFlagCatalog catalog_b;
                public:
                    StatusFlagCatalogTest();
                    ~StatusFlagCatalogTest();
                    bool test();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__434FCB3_385A_4FB4_B40E_654E9B0F6D13_StateFlagCatalogTest_h */
