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

#ifndef __CHAOSFramework__D7B878E_3F06_4472_8769_442D263D93ED_ExternalUnitManager_h
#define __CHAOSFramework__D7B878E_3F06_4472_8769_442D263D93ED_ExternalUnitManager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/ObjectInstancer.h>

#include <chaos/common/external_unit/AbstractServerAdapter.h>
#include <chaos/common/external_unit/AbstractClientAdapter.h>
#include <chaos/common/external_unit/ExternalUnitClientEndpoint.h>
#include <chaos/common/external_unit/ExternalUnitServerEndpoint.h>
#include <chaos/common/external_unit/ExternalEchoEndpoint.h>
#include <chaos/common/external_unit/serialization/AbstractExternalSerialization.h>

#include <algorithm>

namespace chaos{
    namespace common {
        namespace external_unit {
            
            //! define adapter map
            typedef std::pair< ChaosSharedPtr<AbstractServerAdapter> , ChaosSharedPtr<AbstractClientAdapter> > PairAdapter;
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, PairAdapter, MapAdapter);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapAdapter, LMapAdapter);

            
            //!define serialization map
            typedef ChaosSharedPtr< chaos::common::utility::ObjectInstancer<serialization::AbstractExternalSerialization> > ExtSerializerShrdPtr;
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string,  ExtSerializerShrdPtr, MapSerializer);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapSerializer, LMapSerializer);
            
#define CU_EG_SERIALIZER_INSTANCER(SerializerClass)\
new chaos::common::utility::TypedObjectInstancer<SerializerClass, serialization::AbstractExternalSerialization>()
            
            //!External gateway root class
            class ExternalUnitManager:
            public chaos::common::utility::Singleton<ExternalUnitManager>,
            public chaos::common::utility::InizializableService {
                friend class chaos::common::utility::Singleton<ExternalUnitManager>;
                //!anc 'echo' endpoint for testing purphose
                ExternalEchoEndpoint echo_endpoint;
                //!associate the protocol string to the adapter
                LMapAdapter map_protocol_adapter;
                
                LMapSerializer map_serializer;
                
                ExternalUnitManager();
                ~ExternalUnitManager();
                
            public:
                void init(void *init_data) ;
                void deinit() ;
                //!endpoint management api
                //! register an endpoint on all server adapter
                int registerEndpoint(ExternalUnitServerEndpoint& endpoint);
                //! deregister an endpoint on all server protocol adapter
                int deregisterEndpoint(ExternalUnitServerEndpoint& endpoint);
                
                //! initiator management api
                //! create a new connection for and endpoint
                int initilizeConnection(ExternalUnitClientEndpoint& endpoint,
                                        const std::string& protocol,
                                        const std::string& serialization,
                                        const std::string& destination);
                //! dispose a conenction associated to an endpoint
                int releaseConnection(ExternalUnitClientEndpoint& endpoint,
                                      const std::string& protocol);
                
                ChaosUniquePtr<serialization::AbstractExternalSerialization> getNewSerializationInstanceForType(const std::string& type);
                
                template<typename T>
                void installSerializerInstancer() {
                    LMapSerializerWriteLock wl = map_serializer.getWriteLockObject();
                    std::string lowering_type = T::ex_serialization_type;
                    std::transform(lowering_type.begin(), lowering_type.end(), lowering_type.begin(), ::tolower);
                    map_serializer().insert(MapSerializerPair(lowering_type, ExtSerializerShrdPtr(CU_EG_SERIALIZER_INSTANCER(T))));
                }
            };
        }
    }
}

#endif /* __CHAOSFramework__D7B878E_3F06_4472_8769_442D263D93ED_ExternalUnitManager_h */
