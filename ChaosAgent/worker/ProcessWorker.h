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

#ifndef __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessWorker_h
#define __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessWorker_h

#include "../AbstractWorker.h"
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/async_central/async_central.h>

#include <chaos_service_common/data/node/Agent.h>

namespace chaos {
    namespace agent {
        class AgentRegister;
        namespace worker {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, chaos::service_common::data::agent::AgentAssociation, MapRespawnableNode);
            
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapRespawnableNode, LockableMapRespawnableNode);
            
            //! define the agent taht perform operation on the host process
            class ProcessWorker:
            public AbstractWorker,
            public chaos::common::async_central::TimerHandler {
                friend class chaos::agent::AgentRegister;
                LockableMapRespawnableNode map_respawnable_node;
            protected:
                //! launch an data service
                chaos::common::data::CDWUniquePtr launchNode(chaos::common::data::CDWUniquePtr data);
                //! stop an data service
                chaos::common::data::CDWUniquePtr stopNode(chaos::common::data::CDWUniquePtr data);
                //! restart an data service
                chaos::common::data::CDWUniquePtr restartNode(chaos::common::data::CDWUniquePtr data);
                //! list all data service managed by the running instance
                chaos::common::data::CDWUniquePtr checkNodes(chaos::common::data::CDWUniquePtr data);
                //!inherited by @TimerHandler
                void timeout();
            public:
                ProcessWorker();
                ~ProcessWorker();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
                
                void addToRespawn(const chaos::service_common::data::agent::AgentAssociation& node_association_info);
                
                void removeToRespawn(const std::string& node_uid);
                
            };
        }
    }
}

#endif /* __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessWorker_h */
