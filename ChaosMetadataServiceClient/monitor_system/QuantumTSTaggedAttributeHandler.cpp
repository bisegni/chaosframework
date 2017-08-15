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
                         ChaosSharedPtr<SerializationBuffer>(new SerializationBuffer(value->getBinaryValue(attribute, buf_size), buf_size)));
}
