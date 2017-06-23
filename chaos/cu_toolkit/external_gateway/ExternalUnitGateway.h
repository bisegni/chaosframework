/*
 *	ExternalUnitGateway.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 22/06/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__D7B878E_3F06_4472_8769_442D263D93ED_ExternalUnitGateway_h
#define __CHAOSFramework__D7B878E_3F06_4472_8769_442D263D93ED_ExternalUnitGateway_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/cu_toolkit/external_gateway/AbstractAdapter.h>

namespace chaos{
    namespace cu {
        namespace external_gateway {
            
            //! define adapter map
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr<AbstractAdapter>, MapAdapter);
            
            //!External gateway root class
            class ExternalUnitGateway:
            public chaos::common::utility::Singleton<ExternalUnitGateway>,
            public chaos::common::utility::InizializableService {
                friend class chaos::common::utility::Singleton<ExternalUnitGateway>;
                //!associate the protocol string to the adapter
                MapAdapter map_protocol_adapter;
                ExternalUnitGateway();
                ~ExternalUnitGateway();
            public:
                void init(void *init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
            };
        }
    }
}

#endif /* __CHAOSFramework__D7B878E_3F06_4472_8769_442D263D93ED_ExternalUnitGateway_h */
