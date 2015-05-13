/*
 *	AbstractQuantumKeyAttributeHandler.h
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

#ifndef __CHAOSFramework__AbstractQuantumKeyAttributeHandler__
#define __CHAOSFramework__AbstractQuantumKeyAttributeHandler__

#include <ChaosMetadataServiceClient/monitor_system/monitor_system_types.h>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            
#define QuantumKeyAttributeHandlerIMPL_CONST_DIST(x)\
x::x(const std::string& _attribute):\
AbstractQuantumKeyAttributeHandler(_attribute){}\
x::~x(){}\

            //!forward decalration
            class MonitorManager;
            class QuantumSlotScheduler;
            class QuantumKeyConsumer;
            
            class AbstractQuantumKeyAttributeHandler {
                friend class MonitorManager;
                friend class QuantumKeyConsumer;
                //attribute that is managed by this handler
                virtual void _consumeValue(const std::string& key, const KeyValue& value) = 0;
            protected:
                const std::string attribute;
            public:
                AbstractQuantumKeyAttributeHandler(const std::string& _attribute);
                virtual ~AbstractQuantumKeyAttributeHandler();
            };
            
            class QuantumKeyAttributeInt32Handler:
            public AbstractQuantumKeyAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                QuantumKeyAttributeInt32Handler(const std::string& _attribute);
                ~QuantumKeyAttributeInt32Handler();
                virtual void consumeValue(const std::string& key,
                                          const std::string& attribute,
                                          const int32_t value) = 0;
            };
            
            class QuantumKeyAttributeInt64Handler:
            public AbstractQuantumKeyAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                QuantumKeyAttributeInt64Handler(const std::string& _attribute);
                ~QuantumKeyAttributeInt64Handler();
                virtual void consumeValue(const std::string& key,
                                          const std::string& attribute,
                                          const int64_t value) = 0;
            };
            
            class QuantumKeyAttributeDoubleHandler:
            public AbstractQuantumKeyAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                QuantumKeyAttributeDoubleHandler(const std::string& _attribute);
                ~QuantumKeyAttributeDoubleHandler();
                virtual void consumeValue(const std::string& key,
                                          const std::string& attribute,
                                          const double value) = 0;
            };
            
            class QuantumKeyAttributeStringHandler:
            public AbstractQuantumKeyAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                QuantumKeyAttributeStringHandler(const std::string& _attribute);
                ~QuantumKeyAttributeStringHandler();
                virtual void consumeValue(const std::string& key,
                                          const std::string& attribute,
                                          const std::string& value) = 0;
            };
            
            class QuantumKeyAttributeBinaryHandler:
            public AbstractQuantumKeyAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                QuantumKeyAttributeBinaryHandler(const std::string& _attribute);
                ~QuantumKeyAttributeBinaryHandler();
                virtual void consumeValue(const std::string& key,
                                          const std::string& attribute,
                                          const char * value,
                                          const int32_t value_size) = 0;
            };
            
            class QuantumKeyAttributeBoolHandler:
            public AbstractQuantumKeyAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                QuantumKeyAttributeBoolHandler(const std::string& _attribute);
                ~QuantumKeyAttributeBoolHandler();
                virtual void consumeValue(const std::string& key,
                                          const std::string& attribute,
                                          const bool value) = 0;
            };
        }
    }
}
#endif /* defined(__CHAOSFramework__AbstractQuantumKeyAttributeHandler__) */
