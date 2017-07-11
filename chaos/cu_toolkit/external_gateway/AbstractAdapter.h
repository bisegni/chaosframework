/*
 *	AbstractAdapter.h
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

#ifndef __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractAdapter_h
#define __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractAdapter_h

#include <chaos/common/utility/InizializableService.h>

#include <chaos/cu_toolkit/external_gateway/ExternalUnitEndpoint.h>

namespace chaos{
    namespace cu {
        namespace external_gateway {
            //!adapter interface
            class AbstractAdapter:
            public chaos::common::utility::InizializableService {
            public:
                AbstractAdapter();
                ~AbstractAdapter();
                void init(void *init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
                virtual int registerEndpoint(const ExternalUnitEndpoint& endpoint) = 0;
                virtual int deregisterEndpoint(const ExternalUnitEndpoint& endpoint) = 0;
            };
        }
    }
}

#endif /* __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractAdapter_h */
