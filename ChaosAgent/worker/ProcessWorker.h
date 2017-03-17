/*
 *	ProcessWorker.h
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

#ifndef __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessWorker_h
#define __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessWorker_h

#include "../AbstractWorker.h"

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/async_central/async_central.h>

#include <chaos_service_common/data/node/Agent.h>

namespace chaos {
    namespace agent {
        namespace worker {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, chaos::service_common::data::agent::AgentAssociation, MapRespawnableNode);
            
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapRespawnableNode, LockableMapRespawnableNode);
            
            //! define the agent taht perform operation on the host process
            class ProcessWorker:
            public AbstractWorker,
            public chaos::common::async_central::TimerHandler {
                LockableMapRespawnableNode map_respawnable_node;
            protected:
                //! launch an data service
                chaos::common::data::CDataWrapper *launchNode(chaos::common::data::CDataWrapper *data,
                                                              bool& detach);
                //! stop an data service
                chaos::common::data::CDataWrapper *stopNode(chaos::common::data::CDataWrapper *data,
                                                            bool& detach);
                //! restart an data service
                chaos::common::data::CDataWrapper *restartNode(chaos::common::data::CDataWrapper *data,
                                                               bool& detach);
                //! list all data service managed by the running instance
                chaos::common::data::CDataWrapper *checkNodes(chaos::common::data::CDataWrapper *data,
                                                              bool& detach);
                //!inherited by @TimerHandler
                void timeout();
            public:
                ProcessWorker();
                ~ProcessWorker();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
                void launchProcess(const chaos::service_common::data::agent::AgentAssociation& node_association_info);
                bool checkProcessAlive(const chaos::service_common::data::agent::AgentAssociation& node_association_info);
                bool quitProcess(const chaos::service_common::data::agent::AgentAssociation& node_association_info,
                                 bool kill = false);
                
                void addToRespawn(const chaos::service_common::data::agent::AgentAssociation& node_association_info);
                
                void removeToRespawn(const std::string& node_uid);
                
            };
        }
    }
}

#endif /* __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessWorker_h */
