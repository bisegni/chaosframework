/*
 *  mds_types.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#ifndef CHAOSFramework_mds_types_h
#define CHAOSFramework_mds_types_h

#include <map>
#include <vector>
#include <string>

#include "batch/mds_service_batch.h"
#include "persistence/persistence.h"

#include <chaos/common/utility/StartableService.h>

namespace chaos{
	namespace metadata_service {
		
		struct setting {
			std::string							persistence_implementation;
			std::vector<std::string>			persistence_server_list;
			std::map<std::string, std::string>	persistence_kv_param_map;
		};
        
        struct ApiSubserviceAccessor {
            //! network broker for talk with others chaos node
            common::utility::StartableServiceContainer<chaos::common::network::NetworkBroker> network_broker_service;
            
            //! batch executor engine
            common::utility::StartableServiceContainer<batch::MDSBatchExecutor> batch_executor;
            
            //! persistence driver instance
            common::utility::InizializableServiceContainer<chaos::service_common::persistence::data_access::AbstractPersistenceDriver> persistence_driver;
        };
		
	}
}
#endif
