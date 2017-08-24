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

#ifndef __CHAOSFramework__A6646B9_6E82_418D_B7A4_617A53271BEA_ExternalEndpoint_h
#define __CHAOSFramework__A6646B9_6E82_418D_B7A4_617A53271BEA_ExternalEndpoint_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/LockableObject.h>

#include <chaos/cu_toolkit/external_gateway/ExternalUnitConnection.h>

namespace chaos{
    namespace cu {
        namespace external_gateway {
            
            //! Identify and external endpoint
            /*!
             an external unit endpoint is a remote end for a client
             that need to implement a controlunit or a driver. Each
             ednpoint can be confiugre to permit one or multiple connection
             */
            class ExternalUnitEndpoint {
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
                //! end point identifier
                std::string endpoint_identifier;
                
                //!notify a new arrived connection
                virtual void handleNewConnection(const std::string& connection_identifier) = 0;
                
                //! notify that a message has been received for a remote connection
                virtual int handleReceivedeMessage(const std::string& connection_identifier,
                                                   chaos::common::data::CDWUniquePtr message) = 0;
                
                //!notify that a connection has been closed
                virtual void handleDisconnection(const std::string& connection_identifier) = 0;
                
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
                
                ExternalUnitEndpoint();
            public:
                ExternalUnitEndpoint(const std::string& _endpoint_identifier);
                virtual ~ExternalUnitEndpoint();
                const std::string& getIdentifier();
                
                const bool canAcceptMoreConnection();
                
                const int getNumberOfAcceptedConnection() const;
            };
        }
    }
}

#endif /* __CHAOSFramework__A6646B9_6E82_418D_B7A4_617A53271BEA_ExternalEndpoint_h */
