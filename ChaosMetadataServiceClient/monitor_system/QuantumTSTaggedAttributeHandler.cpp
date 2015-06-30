/*
 *	QuantumTSTaggedBoolAttributeHandler.cpp
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


#include <ChaosMetadataServiceClient/monitor_system/QuantumTSTaggedAttributeHandler.h>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::monitor_system;


AbstractQuantumTSTaggedAttributeHandler::AbstractQuantumTSTaggedAttributeHandler(const std::string& _attribute,
                                                                                 const std::string& _ts_attribute,
                                                                                 bool _event_on_value_change):
AbstractQuantumKeyAttributeHandler(_attribute,
                                   _event_on_value_change),
ts_attribute(_ts_attribute){}

AbstractQuantumTSTaggedAttributeHandler::~AbstractQuantumTSTaggedAttributeHandler() {}

//---------------------------implemented handler-------------------------
AbstractQuantumTSTaggedAttributeHandlerIMPL_CONST_DIST(QuantumTSTaggedBoolAttributeHandler)
void QuantumTSTaggedBoolAttributeHandler::_consumeValue(const std::string& key,
                                                        const KeyValue& value) {
    consumeTSTaggedValue(key,
                         attribute,
                         value->getUInt64Value(ts_attribute),
                         value->getBoolValue(attribute));
}

AbstractQuantumTSTaggedAttributeHandlerIMPL_CONST_DIST(QuantumTSTaggedInt32AttributeHandler)
void QuantumTSTaggedInt32AttributeHandler::_consumeValue(const std::string& key,
                                                         const KeyValue& value) {
    consumeTSTaggedValue(key,
                         attribute,
                         value->getUInt64Value(ts_attribute),
                         value->getInt32Value(attribute));
}

AbstractQuantumTSTaggedAttributeHandlerIMPL_CONST_DIST(QuantumTSTaggedInt64AttributeHandler)
void QuantumTSTaggedInt64AttributeHandler::_consumeValue(const std::string& key,
                                                         const KeyValue& value) {
    consumeTSTaggedValue(key,
                         attribute,
                         value->getUInt64Value(ts_attribute),
                         value->getInt64Value(attribute));
}

AbstractQuantumTSTaggedAttributeHandlerIMPL_CONST_DIST(QuantumTSTaggedDoubleAttributeHandler)
void QuantumTSTaggedDoubleAttributeHandler::_consumeValue(const std::string& key,
                                                         const KeyValue& value) {
    consumeTSTaggedValue(key,
                         attribute,
                         value->getUInt64Value(ts_attribute),
                         value->getDoubleValue(attribute));
}

AbstractQuantumTSTaggedAttributeHandlerIMPL_CONST_DIST(QuantumTSTaggedStringAttributeHandler)
void QuantumTSTaggedStringAttributeHandler::_consumeValue(const std::string& key,
                                                          const KeyValue& value) {
    consumeTSTaggedValue(key,
                         attribute,
                         value->getUInt64Value(ts_attribute),
                         value->getStringValue(attribute));
}

AbstractQuantumTSTaggedAttributeHandlerIMPL_CONST_DIST(QuantumTSTaggedBinaryAttributeHandler)
void QuantumTSTaggedBinaryAttributeHandler::_consumeValue(const std::string& key,
                                                          const KeyValue& value) {
    int32_t buf_size = 0;
    consumeTSTaggedValue(key,
                         attribute,
                         value->getUInt64Value(ts_attribute),
                         boost::shared_ptr<SerializationBuffer>(new SerializationBuffer(value->getBinaryValue(attribute, buf_size), buf_size)));
}