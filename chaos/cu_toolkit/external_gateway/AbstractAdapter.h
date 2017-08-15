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

#ifndef __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractAdapter_h
#define __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractAdapter_h

#include <chaos/common/utility/InizializableService.h>

#include <chaos/cu_toolkit/external_gateway/ExternalUnitEndpoint.h>

namespace chaos{
    namespace cu {
        namespace external_gateway {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ExternalUnitEndpoint*, MapEndpoint);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapEndpoint, LMapEndpoint);
            
            //!adapter interface
            class AbstractAdapter:
            public chaos::common::utility::InizializableService {
            protected:
                //!contains all association by endpoint url and class
                LMapEndpoint    map_endpoint;
            public:
                AbstractAdapter();
                ~AbstractAdapter();
                void init(void *init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
                virtual int registerEndpoint(ExternalUnitEndpoint& endpoint) = 0;
                virtual int deregisterEndpoint(ExternalUnitEndpoint& endpoint) = 0;
            };
        }
    }
}

#endif /* __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractAdapter_h */
