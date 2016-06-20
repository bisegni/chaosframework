/*
 *	ExecutionPool.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 20/06/16 INFN, National Institute of Nuclear Physics
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
 *    	limitations under the License.b
 */

#include <chaos/common/configuration/GlobalConfiguration.h>

#include <chaos/cu_toolkit/control_manager/execution_pool/ExecutionPool.h>

using namespace chaos;
using namespace chaos::cu::control_manager::execution_pool;

ExecutionPoolManager::ExecutionPoolManager() {
    
}

ExecutionPoolManager::~ExecutionPoolManager() {
    
}

void ExecutionPoolManager::init(void *init_data) throw(chaos::CException) {
    const std::vector<std::string> e_pool = GlobalConfiguration::getInstance()->getOption< std::vector<std::string> >(CONTROL_MANAGER_EXECUTION_POOLS);
}


void ExecutionPoolManager::deinit() throw(chaos::CException) {
    
}
