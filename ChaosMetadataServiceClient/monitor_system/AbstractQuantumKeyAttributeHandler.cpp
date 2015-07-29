/*
 *	AbstractQuantumKeyAttributeHandler.cpp
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

#include <ChaosMetadataServiceClient/monitor_system/AbstractQuantumKeyAttributeHandler.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::monitor_system;

AbstractQuantumKeyAttributeHandler::AbstractQuantumKeyAttributeHandler(const std::string& _attribute,
                                                                       bool  _event_on_value_change):
attribute(_attribute),
event_on_value_change(_event_on_value_change) {}

AbstractQuantumKeyAttributeHandler::~AbstractQuantumKeyAttributeHandler() {}

const std::string& AbstractQuantumKeyAttributeHandler::getAttributeName() {
    return attribute;
}

void AbstractQuantumKeyAttributeHandler::_consumeValueNotFound(const std::string& key) {
    consumeValueNotFound(key,
                         attribute);
}

void AbstractQuantumKeyAttributeHandler::consumeValueNotFound(const std::string& key,
                                                              const std::string& attribute) {
    
}
//---------------------------implemented handler-------------------------
QuantumKeyAttributeHandlerIMPL_CONST_DIST(QuantumKeyAttributeBoolHandler)
void QuantumKeyAttributeBoolHandler::_consumeValue(const std::string& key,
                                                   const KeyValue& value) {

    consumeValue(key,
                 attribute,
                 value->getBoolValue(attribute));
}

QuantumKeyAttributeHandlerIMPL_CONST_DIST(QuantumKeyAttributeInt32Handler)
void QuantumKeyAttributeInt32Handler::_consumeValue(const std::string& key,
                                                    const KeyValue& value) {
    consumeValue(key,
                 attribute,
                 value->getInt32Value(attribute));
}

QuantumKeyAttributeHandlerIMPL_CONST_DIST(QuantumKeyAttributeInt64Handler)
void QuantumKeyAttributeInt64Handler::_consumeValue(const std::string& key,
                                                    const KeyValue& value) {
    consumeValue(key,
                 attribute,
                 value->getInt64Value(attribute));
}

QuantumKeyAttributeHandlerIMPL_CONST_DIST(QuantumKeyAttributeDoubleHandler)
void QuantumKeyAttributeDoubleHandler::_consumeValue(const std::string& key,
                                                     const KeyValue& value) {
    consumeValue(key,
                 attribute,
                 value->getDoubleValue(attribute));
}

QuantumKeyAttributeHandlerIMPL_CONST_DIST(QuantumKeyAttributeStringHandler)
void QuantumKeyAttributeStringHandler::_consumeValue(const std::string& key,
                                                     const KeyValue& value) {
    consumeValue(key,
                 attribute,
                 value->getStringValue(attribute));

}

QuantumKeyAttributeHandlerIMPL_CONST_DIST(QuantumKeyAttributeBinaryHandler)
void QuantumKeyAttributeBinaryHandler::_consumeValue(const std::string& key,
                                                     const KeyValue& value) {
    int32_t buf_size = 0;
    consumeValue(key,
                 attribute,
                 boost::shared_ptr<SerializationBuffer>(new SerializationBuffer(value->getBinaryValue(attribute, buf_size), buf_size)));
}



