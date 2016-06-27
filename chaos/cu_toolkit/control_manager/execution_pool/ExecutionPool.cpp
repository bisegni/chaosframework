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

#include <chaos/cu_toolkit/control_manager/execution_pool/ExecutionPool.h>

#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;
using namespace chaos::common::healt_system;
using namespace chaos::common::async_central;
using namespace chaos::cu::control_manager::execution_pool;

#define INFOL INFO_LOG(ExecutionPoolManager)
#define ERRL ERR_LOG(ExecutionPoolManager)
#define DBGL DBG_LOG(ExecutionPoolManager)


ExecutionPoolManager::ExecutionPoolManager():
cpu_cap_percentage(CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DEFAULT){}

ExecutionPoolManager::~ExecutionPoolManager() {}

void ExecutionPoolManager::init(void *init_data) throw(chaos::CException) {
    int err = 0;
    execution_pool_list = GlobalConfiguration::getInstance()->getOption< std::vector<std::string> >(CONTROL_MANAGER_EXECUTION_POOLS);
    
    cpu_cap_percentage = GlobalConfiguration::getInstance()->getOption< double >(CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP);
    
    if((err = AsyncCentralManager::getInstance()->addTimer(this,
                                                           CONTROL_MANAGER_EXECUTION_POOLS_CHECK_TIME,
                                                           CONTROL_MANAGER_EXECUTION_POOLS_CHECK_TIME))) {
        LOG_AND_TROW(ERR_LOG(ExecutionPoolManager), err, "Error adding timer for execution pools management");
    }
}


void ExecutionPoolManager::deinit() throw(chaos::CException) {
    AsyncCentralManager::getInstance()->removeTimer(this);
}

void ExecutionPoolManager::registerUID(const std::string& new_uid) {
    boost::unique_lock<boost::mutex> wl(mutex_uid_set);
    eu_uid_list.insert(new_uid);
}

void ExecutionPoolManager::deregisterUID(const std::string& remove_uid) {
    boost::unique_lock<boost::mutex> wl(mutex_uid_set);
    eu_uid_list.erase(remove_uid);
}

void ExecutionPoolManager::timeout() {
    const ProcInfo cur_proc_stat = HealtManager::getInstance()->getLastProcInfo();
    // make heart beat for the managed eu uid
    
    // check if we have enought resources for run script
    if(cur_proc_stat <= cpu_cap_percentage) {
        // request new job for contained pool
        DBGL << CHAOS_FORMAT("Tell MDS that we have want other script to execute(cpu res/cap[%1%-%2%])!", %cur_proc_stat%cpu_cap_percentage);
    }
}
