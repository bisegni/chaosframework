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

#include <chaos/cu_toolkit/control_manager/execution_pool/ExecutionPool.h>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;

using namespace chaos::common::healt_system;
using namespace chaos::common::async_central;
using namespace chaos::cu::control_manager::execution_pool;

#define INFOL INFO_LOG(ExecutionPoolManager)
#define ERRL ERR_LOG(ExecutionPoolManager)
#define DBGL DBG_LOG(ExecutionPoolManager)


ExecutionPoolManager::ExecutionPoolManager():
cpu_cap_percentage(CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DEFAULT),
mds_message_channel(NULL){}

ExecutionPoolManager::~ExecutionPoolManager() {}

void ExecutionPoolManager::init(void *init_data)  {
    int err = 0;
    unit_server_alias = GlobalConfiguration::getInstance()->getOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_ALIAS);
    
    execution_pool_list = GlobalConfiguration::getInstance()->getOption< std::vector<std::string> >(CONTROL_MANAGER_EXECUTION_POOLS);
    
    cpu_cap_percentage = GlobalConfiguration::getInstance()->getOption< double >(CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP);
    
    if((mds_message_channel = NetworkBroker::getInstance()->getMetadataserverMessageChannel()) == NULL){
        LOG_AND_TROW(ERR_LOG(ExecutionPoolManager), -1, "Error allocating message channel");
    }
    
    if((err = AsyncCentralManager::getInstance()->addTimer(this,
                                                           0,
                                                           CONTROL_MANAGER_EXECUTION_POOLS_CHECK_TIME))) {
        LOG_AND_TROW(ERR_LOG(ExecutionPoolManager), err, "Error adding timer for execution pools management");
    }
}


void ExecutionPoolManager::deinit()  {
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
    boost::unique_lock<boost::mutex> wl(mutex_uid_set);
    if(eu_uid_list.size() == 0 &&
       execution_pool_list.size() == 0) return;
    
    CDWUniquePtr hb_message(new CDataWrapper());
    //collect proc stat
    const ProcStat cur_proc_stat = HealtManager::getInstance()->getLastProcInfo();
    const double total_cpu_usage = cur_proc_stat.sys_time+cur_proc_stat.usr_time;
    
    //ad current unit server alias
    hb_message->addStringValue(chaos::NodeDefinitionKey::NODE_PARENT,
                              unit_server_alias);
    /*if(eu_uid_list.size()) {
        // make heart beat for the managed eu uid
        for(ChaosStringSetIterator it = eu_uid_list.begin(),
            end = eu_uid_list.end();
            it != end;
            it++) {
            hb_message.appendStringToArray(*it);
        }
        hb_message.finalizeArrayForKey(ExecutionUnitNodeDefinitionKey::EXECUTION_SCRIPT_INSTANCE_LIST);
    }*/
    
    // check if we have enought resources for run script
    if(total_cpu_usage < cpu_cap_percentage &&
       execution_pool_list.size()) {
        // request new job for contained pool
        DBGL << CHAOS_FORMAT("Tell MDS that we have want other script to execute(cpu res/cap[%1%-%2%])!", %total_cpu_usage%cpu_cap_percentage);
        
        //in this case we need to forward also the connection pools
        for(ChaosStringVectorIterator it = execution_pool_list.begin(),
            end = execution_pool_list.end();
            it != end;
            it++) {
            hb_message->appendStringToArray(*it);
        }
        hb_message->finalizeArrayForKey(ExecutionUnitNodeDefinitionKey::EXECUTION_POOL_LIST);
    }
    
    //send message to mds
    mds_message_channel->sendMessage("script",
                                     "executionPoolHeartbeat",
                                     MOVE(hb_message));
}
