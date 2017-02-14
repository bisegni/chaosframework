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

#include <chaos_service_common/data/node/Agent.h>

namespace chaos {
    namespace agent {
        namespace worker {
            
            //! define the agent taht perform operation on the host process
            class ProcessWorker:
            public AbstractWorker {
                std::string launchProcess(const chaos::service_common::data::agent::AgentAssociation& node_association_info);
                bool checkProcessAlive(const chaos::service_common::data::agent::AgentAssociation& node_association_info);
            protected:
                //! launch an data service
                chaos::common::data::CDataWrapper *launchUnitServer(chaos::common::data::CDataWrapper *data,
                                                                    bool& detach);
                //! stop an data service
                chaos::common::data::CDataWrapper *stopUnitServer(chaos::common::data::CDataWrapper *data,
                                                                  bool& detach);
                //! restart an data service
                chaos::common::data::CDataWrapper *restartUnitServer(chaos::common::data::CDataWrapper *data,
                                                                     bool& detach);
                //! list all data service managed by the running instance
                chaos::common::data::CDataWrapper *listUnitServers(chaos::common::data::CDataWrapper *data,
                                                                   bool& detach);
            public:
                ProcessWorker();
                ~ProcessWorker();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
            };
        }
    }
}

#endif /* __CHAOSFramework_BED84E1E_63AB_431A_9CDD_DC38C4187D19_ProcessWorker_h */
