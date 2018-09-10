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

#ifndef __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractServerAdapter_h
#define __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractServerAdapter_h
#include <chaos/common/external_unit/ExternalUnitServerEndpoint.h>
#include <chaos/common/external_unit/ExternalUnitConnection.h>
#include <chaos/common/external_unit/AbstractAdapter.h>

namespace chaos{
    namespace common {
        namespace external_unit {
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ExternalUnitServerEndpoint*, MapEndpoint);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapEndpoint, LMapEndpoint);
            
            //!adapter interface
            class AbstractServerAdapter:
            public AbstractAdapter {
            protected:
                //!contains all association by endpoint url and class
                LMapEndpoint    map_endpoint;
            protected:
                int sendDataToEndpoint(ExternalUnitConnection& connection, chaos::common::data::CDBufferUniquePtr received_data);
            public:
                AbstractServerAdapter();
                ~AbstractServerAdapter();
                void init(void *init_data) ;
                void deinit() ;
                
                virtual int registerEndpoint(ExternalUnitServerEndpoint& endpoint) = 0;
                virtual int deregisterEndpoint(ExternalUnitServerEndpoint& endpoint) = 0;
                
                virtual int sendDataToConnection(const std::string& connection_identifier,
                                                 const chaos::common::data::CDBufferUniquePtr data,
                                                 const EUCMessageOpcode opcode) = 0;
                
                virtual int closeConnection(const std::string& connection_identifier) = 0;
            };
        }
    }
}

#endif /* __CHAOSFramework__03715EB_48C9_4FC9_933D_9B908EC5B726_AbstractServerAdapter_h */
