/*
 *	QuantumTypedConsumer.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__QuantumTypedConsumer__
#define __CHAOSFramework__QuantumTypedConsumer__

#include <ChaosMetadataServiceClient/monitor_system/monitor_system_types.h>
#include <ChaosMetadataServiceClient/monitor_system/AbstractQuantumKeyAttributeHandler.h>

#include <boost/thread.hpp>

#include <map>
#include <string>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            //!forward declaration
            class QuantumSlotScheduler;
            
            typedef std::map<uintptr_t, AbstractQuantumKeyAttributeHandler* >             AttributeHandlerMap;
            typedef std::map<uintptr_t, AbstractQuantumKeyAttributeHandler* >::iterator   AttributeHandlerMapIterator;
            
            //! is a consumer for a keythat manager many handler for the key attribute
            class QuantumKeyConsumer:
            public QuantumSlotConsumer {
                friend class QuantumSlotScheduler;
                const std::string       key;
                boost::mutex            map_mutex;
                AttributeHandlerMap     map_attribute_handler;
                
            protected:
                //! called when data for key has been retrieved from chaos data service
                void quantumSlotHasData(const std::string& key,
                                        const KeyValue& value);
                //! callend when no data has ben retrived from data service(key not found or no cds available)
                void quantumSlotHasNoData(const std::string& key);
            public:
                QuantumKeyConsumer(const std::string& _key);
                ~QuantumKeyConsumer();
                void addAttributeHandler(AbstractQuantumKeyAttributeHandler *handler);
                
                void removeAttributeHandler(AbstractQuantumKeyAttributeHandler *handler);
                
                const std::string& getKey();
                
                const size_t size();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__QuantumTypedConsumer__) */
