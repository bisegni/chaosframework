/*
 * Copyright 2012, 10/10/2017 INFN
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

#ifndef chaos_common_external_unit_UnitEndpoint_h
#define chaos_common_external_unit_UnitEndpoint_h

#include <string>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/external_unit/ExternalUnitConnection.h>
namespace chaos{
    namespace common {
        namespace external_unit {
            class ExternalUnitConnection;
            //!adapter interface
            class UnitEndpoint {
                friend class ExternalUnitConnection;
            protected:
                //! notify that a message has been received for a remote connection
                virtual int handleReceivedeMessage(const std::string& connection_identifier,
                                                   chaos::common::data::CDWUniquePtr message) = 0;
                //! send a message to a connection
                virtual int addConnection(ExternalUnitConnection& new_connection) = 0;
                
                //! send a message to a connection
                virtual int removeConnection(ExternalUnitConnection& removed_connection) = 0;
            public:
                virtual std::string getIdentifier() = 0;
            };
        }
    }
}
#endif /* chaos_common_external_unit_UnitEndpoint_h */
