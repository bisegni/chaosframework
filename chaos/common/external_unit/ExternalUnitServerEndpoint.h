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

#ifndef chaos_common_external_unit_ExternalUnitServerEndpoint_h
#define chaos_common_external_unit_ExternalUnitServerEndpoint_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/LockableObject.h>

#include <chaos/common/external_unit/UnitEndpoint.h>
#include <chaos/common/external_unit/ExternalUnitConnection.h>

namespace chaos{
    namespace common {
        namespace external_unit {
            
            //! Identify and external endpoint
            /*!
             an external unit endpoint is a remote end for a client
             that need to implement a controlunit or a driver. Each
             ednpoint can be confiugre to permit one or multiple connection
             */
            class ExternalUnitServerEndpoint:
            public UnitEndpoint {
                friend class ExternalUnitConnection;
                //! define adapter map
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ExternalUnitConnection*, MapConnection);
                
                CHAOS_DEFINE_LOCKABLE_OBJECT(MapConnection, LMapConnection);
                
                LMapConnection map_connection;
                
                int number_of_connection_accepted;
                
                //! send a message to a connection
                int addConnection(ExternalUnitConnection& new_connection);
                
                //! send a message to a connection
                int removeConnection(ExternalUnitConnection& removed_connection);
                
            protected:
                std::string endpoint_identifier;
                //! send a message throught a remote connection
                int sendMessage(const std::string& connection_identifier,
                                chaos::common::data::CDWUniquePtr message,
                                const EUCMessageOpcode opcode = EUCMessageOpcodeWhole);
                //! send an error to remote driver
                int sendError(const std::string& connection_identifier,
                              int error_code,
                              const std::string& error_message,
                              const std::string& error_domain);
                //! send an error to remote driver
                int sendError(const std::string& connection_identifier,
                              const chaos::CException& ex);
                
                //!close the connection
                void closeConnection(const std::string& connection_identifier);
                
                void setNumberOfAcceptedConnection(int _number_of_connection_accepted);
                
                ExternalUnitServerEndpoint();
            public:
                ExternalUnitServerEndpoint(const std::string& _endpoint_identifier);
                virtual ~ExternalUnitServerEndpoint();
                
                std::string getIdentifier();
                
                const bool canAcceptMoreConnection();
                
                const int getNumberOfAcceptedConnection() const;
            };
        }
    }
}

#endif /* chaos_common_external_unit_ExternalUnitServerEndpoint_h */
