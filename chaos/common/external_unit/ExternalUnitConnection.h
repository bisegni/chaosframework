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

#ifndef __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h
#define __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h

#include <chaos/common/chaos_types.h>

#include <chaos/common/external_unit/serialization/AbstractExternalSerialization.h>

namespace chaos {
    namespace common {
        namespace external_unit {
            class ExternalUnitEndpoint;
            class AbstractAdapter;
            
            //!define the message opcode used to manage the fragmentation of the data
            typedef enum {
                EUCMessageOpcodeWhole,          //! send in one shot all message data
                EUCPhaseStartFragment,          //!message is the beginning of the fragment
                EUCPhaseContinueFragment,       //!message is the cntinuation of the fragment
                EUCPhaseEndFragment             //!message is the end of the whole fragment
            } EUCMessageOpcode;
            
            //! Identify an external connection
            class ExternalUnitConnection {
                friend class AbstractAdapter;
            protected:
                AbstractAdapter *adapter;
                //!endpoint that own the connection
                ExternalUnitEndpoint *endpoint;
                
                //!,anage the absractio of serialization
                ChaosUniquePtr<chaos::common::external_unit::serialization::AbstractExternalSerialization> serializer_adapter;
            protected:
                int sendDataToEndpoint(chaos::common::data::CDBufferUniquePtr reecived_data);
            public:
                //! end point identifier
                const std::string connection_identifier;
                
                ExternalUnitConnection(AbstractAdapter *_adapter,
                                       ExternalUnitEndpoint *_endpoint,
                                       ChaosUniquePtr<chaos::common::external_unit::serialization::AbstractExternalSerialization> _serializer_adapter);
                virtual ~ExternalUnitConnection();
                
                //! send data over external protocol
                int sendData(chaos::common::data::CDWUniquePtr data,
                             const EUCMessageOpcode opcode = EUCMessageOpcodeWhole);
                
                virtual void closeConnection();
                
                const std::string& getEndpointIdentifier() const;
            };
        }
    }
}

#endif /* __CHAOSFramework_A9E1864B_5C02_4ECA_8289_AFA41CE09C52_ExternalConnection_h */
