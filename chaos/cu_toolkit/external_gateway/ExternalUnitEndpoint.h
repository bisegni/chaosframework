/*
 *	ExternalEndpoint.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 11/07/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__A6646B9_6E82_418D_B7A4_617A53271BEA_ExternalEndpoint_h
#define __CHAOSFramework__A6646B9_6E82_418D_B7A4_617A53271BEA_ExternalEndpoint_h

#include <chaos/common/chaos_types.h>
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

                //! end point identifier
                const std::string endpoint_identifier;
                
                LMapConnection map_connection;
                
                //! send a message to a connection
                int addConnection(ExternalUnitConnection& new_connection);
                
                //! send a message to a connection
                int removeConnection(ExternalUnitConnection& removed_connection);
                
            protected:
                //!notify a new arrived connection
                virtual void handleNewConnection(const std::string& connection_identifier) = 0;
                
                //! notify that a message has been received for a remote connection
                virtual int handleReceivedeMessage(const std::string& connection_identifier, const std::string& message) = 0;
                
                //!notify that a connection has been closed
                virtual void handleDisconnection(const std::string& connection_identifier) = 0;
                
                //! send a message to a connection
                int sendMessage(const std::string& connection_identifier, const std::string& message);
            public:
                ExternalUnitEndpoint(const std::string& _endpoint_identifier);
                virtual ~ExternalUnitEndpoint();
                const std::string& getIdentifier();
            };
        }
    }
}

#endif /* __CHAOSFramework__A6646B9_6E82_418D_B7A4_617A53271BEA_ExternalEndpoint_h */
