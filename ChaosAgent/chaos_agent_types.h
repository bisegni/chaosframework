/*
 *	chaos_agent_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 03/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__257C4FE_DCA8_4943_B997_F7B2F28195CC_chaos_agent_types_h
#define __CHAOSFramework__257C4FE_DCA8_4943_B997_F7B2F28195CC_chaos_agent_types_h

namespace chaos {
    namespace agent {
 
        typedef struct ChaosAgentSettings {
            //!working directory for deploy and execution of chaos binary
            std::string working_directory;
        } ChaosAgentSettings;
        
    }
}

#endif /* __CHAOSFramework__257C4FE_DCA8_4943_B997_F7B2F28195CC_chaos_agent_types_h */
