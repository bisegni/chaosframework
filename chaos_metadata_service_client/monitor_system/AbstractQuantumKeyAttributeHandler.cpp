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

#include <chaos_metadata_service_client/monitor_system/AbstractQuantumKeyAttributeHandler.h>

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
QuantumKeyAttributeHandlerIMPL_CONST_DIST(QuantumKeyAttributeCDataWrapperHandler)
void QuantumKeyAttributeCDataWrapperHandler::_consumeValue(const std::string& key,
                                                     const KeyValue& value) {
	CDataWrapper * tmp=value->getCSDataValue(attribute);
	if(tmp){
		consumeValue(key,
                 attribute,
                 tmp->getJSONString());
		delete tmp;
	}

}
QuantumKeyAttributeHandlerIMPL_CONST_DIST(QuantumKeyAttributeBinaryHandler)
void QuantumKeyAttributeBinaryHandler::_consumeValue(const std::string& key,
                                                     const KeyValue& value) {
    uint32_t buf_size = 0;
    consumeValue(key,
                 attribute,
                 ChaosSharedPtr<SerializationBuffer>(new SerializationBuffer(value->getBinaryValue(attribute, buf_size), buf_size)));
}



