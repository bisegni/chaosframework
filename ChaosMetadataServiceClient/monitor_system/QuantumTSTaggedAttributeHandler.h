/*
 *	QuantumTSTaggedBoolAttributeHandler.h
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

#ifndef __CHAOSFramework__QuantumTSTaggedAttributeHandler__
#define __CHAOSFramework__QuantumTSTaggedAttributeHandler__

#include <ChaosMetadataServiceClient/monitor_system/monitor_system_types.h>
#include <ChaosMetadataServiceClient/monitor_system/AbstractQuantumKeyAttributeHandler.h>

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            
#define AbstractQuantumTSTaggedAttributeHandlerIMPL_CONST_DIST(x)\
x::x(const std::string& _attribute, const std::string& _ts_attribute, bool _event_on_value_change):\
AbstractQuantumTSTaggedAttributeHandler(_attribute, _ts_attribute, _event_on_value_change) {}\
x::~x(){}

#define AbstractQuantumTSTaggedAttributeHandlerDEF_CONST_DIST(x)\
x(const std::string& _attribute, const std::string& _ts_attribute, bool _event_on_value_change = false);\
~x();
            
            //!base handler class for dataset specific class
            /*!
             The difference between the others handler is that in the eventi is also passed the timestamp
             of the datapack, because in every datpak it is present.
             */
            class AbstractQuantumTSTaggedAttributeHandler:
            public AbstractQuantumKeyAttributeHandler {
            protected:
                //define the attribute name associated to the timestamp tag
                const std::string ts_attribute;
            public:
                AbstractQuantumTSTaggedAttributeHandler(const std::string& _attribute,
                                                        const std::string& _ts_attribute,
                                                        bool _event_on_value_change = false);
                ~AbstractQuantumTSTaggedAttributeHandler();
            };
            
            //!bool time stamped tagged handler
            class QuantumTSTaggedBoolAttributeHandler:
            public AbstractQuantumTSTaggedAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                AbstractQuantumTSTaggedAttributeHandlerDEF_CONST_DIST(QuantumTSTaggedBoolAttributeHandler)
                virtual void consumeTSTaggedValue(const std::string& key,
                                                  const std::string& attribute,
                                                  uint64_t timestamp,
                                                  const bool value) = 0;
            };
            
            //!int32 time stamped tagged handler
            class QuantumTSTaggedInt32AttributeHandler:
            public AbstractQuantumTSTaggedAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                AbstractQuantumTSTaggedAttributeHandlerDEF_CONST_DIST(QuantumTSTaggedInt32AttributeHandler)
                virtual void consumeTSTaggedValue(const std::string& key,
                                                  const std::string& attribute,
                                                  uint64_t timestamp,
                                                  const int32_t value) = 0;
            };
            
            //!int64 time stamped tagged handler
            class QuantumTSTaggedInt64AttributeHandler:
            public AbstractQuantumTSTaggedAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                AbstractQuantumTSTaggedAttributeHandlerDEF_CONST_DIST(QuantumTSTaggedInt64AttributeHandler)
                virtual void consumeTSTaggedValue(const std::string& key,
                                                  const std::string& attribute,
                                                  uint64_t timestamp,
                                                  const int64_t value) = 0;
            };
            
            //!double time stamped tagged handler
            class QuantumTSTaggedDoubleAttributeHandler:
            public AbstractQuantumTSTaggedAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                AbstractQuantumTSTaggedAttributeHandlerDEF_CONST_DIST(QuantumTSTaggedDoubleAttributeHandler)
                virtual void consumeTSTaggedValue(const std::string& key,
                                                  const std::string& attribute,
                                                  uint64_t timestamp,
                                                  const double value) = 0;
            };
            
            //!string time stamped tagged handler
            class QuantumTSTaggedStringAttributeHandler:
            public AbstractQuantumTSTaggedAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                AbstractQuantumTSTaggedAttributeHandlerDEF_CONST_DIST(QuantumTSTaggedStringAttributeHandler)
                virtual void consumeTSTaggedValue(const std::string& key,
                                                  const std::string& attribute,
                                                  uint64_t timestamp,
                                                  const std::string& value) = 0;
            };
            
            //!binary time stamped tagged handler
            class QuantumTSTaggedBinaryAttributeHandler:
            public AbstractQuantumTSTaggedAttributeHandler {
                void _consumeValue(const std::string& key, const KeyValue& value);
            public:
                AbstractQuantumTSTaggedAttributeHandlerDEF_CONST_DIST(QuantumTSTaggedBinaryAttributeHandler)
                virtual void consumeTSTaggedValue(const std::string& key,
                                                  const std::string& attribute,
                                                  uint64_t timestamp,
                                                  const boost::shared_ptr<chaos::common::data::SerializationBuffer>& value) = 0;
            };
        }
    }
}
#endif /* defined(__CHAOSFramework__QuantumTSTaggedAttributeHandler__) */
