/*
 *	AbstractAgent.cpp
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

#include "AbstractAgent.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::agent;
using namespace chaos::common::data;

AbstractAgent::AbstractAgent(const std::string& _agent_name):
agent_name(_agent_name){}

AbstractAgent::~AbstractAgent() {}

void AbstractAgent::init(void *data) throw(chaos::CException) {
    
}

void AbstractAgent::deinit() throw(chaos::CException) {
    
}

const std::string& AbstractAgent::getAgentName() const {
    return agent_name;
}
