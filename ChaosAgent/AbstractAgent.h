/*
 *	AbstractAgent.h
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

#ifndef __CHAOSFramework__B8D3722_2599_4337_999F_FADA42E4AC95_AbstractAgent_h
#define __CHAOSFramework__B8D3722_2599_4337_999F_FADA42E4AC95_AbstractAgent_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/InizializableService.h>

#include <memory>

namespace chaos {
    namespace agent {
        
        class AbstractAgent:
        public DeclareAction,
        public chaos::common::utility::InizializableService{
            const std::string agent_name;
        public:
            AbstractAgent(const std::string& _agent_name);
            virtual ~AbstractAgent();
            
            void init(void *data) throw(chaos::CException);
            void deinit() throw(chaos::CException);
            
            const std::string& getAgentName() const;
        };
    }
}

#endif /* __CHAOSFramework__B8D3722_2599_4337_999F_FADA42E4AC95_AbstractAgent_h */
