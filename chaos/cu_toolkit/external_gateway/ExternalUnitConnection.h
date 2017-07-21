/*
 *	ExternalConnection.h
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

#ifndef __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h
#define __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h

#include <chaos/common/chaos_types.h>

#include <chaos/cu_toolkit/external_gateway/serialization/AbstractExternalSerialization.h>

namespace chaos {
    namespace cu {
        namespace external_gateway {
            class AbstractAdapter;
            class ExternalUnitEndpoint;
            
            //!define the message opcode used to manage the fragmentation of the data
            typedef enum {
                EUCMessageOpcodeWhole,          //! send in one shot all message data
                EUCPhaseStartFragment,          //!message is the beginning of the fragment
                EUCPhaseContinueFragment,       //!message is the cntinuation of the fragment
                EUCPhaseEndFragment             //!message is the end of the whole fragment
            } EUCMessageOpcode;
            
            //! Identify an external connection
            class ExternalUnitConnection {
            protected:
                //!endpoint that own the connection
                ExternalUnitEndpoint *endpoint;
                
                //!,anage the absractio of serialization
                ChaosUniquePtr<chaos::cu::external_gateway::serialization::AbstractExternalSerialization> serializer_adapter;
            protected:
                int sendDataToEndpoint(chaos::common::data::CDBufferUniquePtr reecived_data);
                virtual int sendDataToConnection(chaos::common::data::CDBufferUniquePtr data,
                                                 const EUCMessageOpcode opcode = EUCMessageOpcodeWhole) = 0;
            public:
                //! end point identifier
                const std::string connection_identifier;
                
                ExternalUnitConnection(ExternalUnitEndpoint *_endpoint,
                                       ChaosUniquePtr<chaos::cu::external_gateway::serialization::AbstractExternalSerialization> _serializer_adapter);
                virtual ~ExternalUnitConnection();
                
                //! send data over external protocol
                int sendData(chaos::common::data::CDWUniquePtr data,
                             const EUCMessageOpcode opcode = EUCMessageOpcodeWhole);
                
                virtual void closeConnection() = 0;
                
                const std::string& getEndpointIdentifier() const;
            };
        }
    }
}

#endif /* __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h */
