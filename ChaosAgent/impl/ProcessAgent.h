/*
 *	ProcessAgent.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessAgent_h
#define __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessAgent_h

#include "../AbstractAgent.h"


namespace chaos {
    namespace agent {
        namespace impl {
            
            //! define the agent taht perform operation on the host process
            class ProcessAgent:
            public AbstractAgent {
            public:
                ProcessAgent();
                ~ProcessAgent();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
            };
        }
    }
}

#endif /* __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessAgent_h */
