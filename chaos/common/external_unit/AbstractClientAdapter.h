/*
 * Copyright 2012, 11/10/2017 INFN
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

#ifndef chaos_common_external_unit_AbstractClientAdapter_h
#define chaos_common_external_unit_AbstractClientAdapter_h
#include <chaos/common/external_unit/ExternalUnitClientEndpoint.h>
#include <chaos/common/external_unit/ExternalUnitConnection.h>
#include <chaos/common/external_unit/AbstractAdapter.h>

namespace chaos{
    namespace common {
        namespace external_unit {
            
            //!adapter interface
            class AbstractClientAdapter:
            public AbstractAdapter {
            protected:
                int sendDataToEndpoint(ExternalUnitConnection& connection, chaos::common::data::CDBufferUniquePtr received_data);
                virtual int sendDataToConnection(const std::string& connection_identifier,
                                                 const chaos::common::data::CDBufferUniquePtr data,
                                                 const EUCMessageOpcode opcode) = 0;
                
                virtual int closeConnection(const std::string& connection_identifier) = 0;
            public:
                AbstractClientAdapter();
                ~AbstractClientAdapter();
                void init(void *init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
                virtual int addNewConnectionForEndpoint(ExternalUnitClientEndpoint *endpoint,
                                                const std::string& endpoint_url,
                                                const std::string& serialization) = 0;
                virtual int removeConnectionsFromEndpoint(ExternalUnitClientEndpoint *target_endpoint) = 0;
            };
        }
    }
}

#endif /* chaos_common_external_unit_AbstractClientAdapter_h */
