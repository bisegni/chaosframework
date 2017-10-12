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

#include <chaos/common/external_unit/ExternalUnitConnection.h>
#include <chaos/common/external_unit/UnitEndpoint.h>
namespace chaos{
    namespace common {
        namespace external_unit {
            
            //! Identify and external endpoint
            /*!
             an external unit endpoint is a remote end for a client
             that need to implement a controlunit or a driver. Each
             ednpoint can be confiugre to permit one or multiple connection
             */
            class ExternalUnitClientEndpoint:
            public ::chaos::common::external_unit::UnitEndpoint {
                friend class ExternalUnitConnection;
                CHAOS_DEFINE_LOCKABLE_OBJECT(::chaos::common::external_unit::ExternalUnitConnection*, LExternalUnitConnection);
                LExternalUnitConnection current_connection;
                
                //! send a message to a connection
                int setConnection(::chaos::common::external_unit::ExternalUnitConnection& new_connection);
                
                //! send a message to a connection
                int removeConnection(::chaos::common::external_unit::ExternalUnitConnection& removed_connection);
                
            protected:
                
                //! send a message throught a remote connection
                int sendMessage(::chaos::common::data::CDWUniquePtr message,
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
                
                ExternalUnitClientEndpoint();
            public:
                const std::string client_identification;
                ExternalUnitClientEndpoint(const std::string& _client_identification);
                virtual ~ExternalUnitClientEndpoint();
                std::string getIdentifier();
            };
        }
    }
}

#endif /* __CHAOSFramework__A6646B9_6E82_418D_B7A4_617A53271BEA_ExternalEndpoint_h */
