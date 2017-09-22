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

#include "QuantumKeyConsumer.h"

#define QKC_INFO   INFO_LOG (QuantumKeyConsumer)
#define QKC_DBG    DBG_LOG  (QuantumKeyConsumer)
#define QKC_ERR    ERR_LOG  (QuantumKeyConsumer)

using namespace chaos::metadata_service_client::monitor_system;

QuantumKeyConsumer::QuantumKeyConsumer(const std::string& _key):
key(_key){
    
}
QuantumKeyConsumer::~QuantumKeyConsumer() {
    
}

void QuantumKeyConsumer::quantumSlotHasData(const std::string& key,
                                            const KeyValue& value) {
    //acquire read lock
    boost::unique_lock<boost::mutex> wl(map_mutex);
    QKC_DBG<< "Broadcast data to handler for key " << key;
    //scan all attribute and call handler
    for(AttributeHandlerMapIterator it = map_attribute_handler.begin();
        it != map_attribute_handler.end();
        it++) {
        if(value->hasKey(it->second->attribute)){
            //broadcast value
            it->second->_consumeValue(key,
                                      value);
        }
    }
}

void QuantumKeyConsumer::quantumSlotHasNoData(const std::string& key) {
    //acquire read lock
    boost::unique_lock<boost::mutex> wl(map_mutex);
    QKC_DBG<< "Broadcast no data to handler for key " << key;
    //scan all attribute and call handler
    for(AttributeHandlerMapIterator it = map_attribute_handler.begin();
        it != map_attribute_handler.end();
        it++) {
            //broadcast value
            it->second->_consumeValueNotFound(key);
    }
}

void QuantumKeyConsumer::addAttributeHandler(AbstractQuantumKeyAttributeHandler *handler) {
    //aquire write lock to work on map
    boost::unique_lock<boost::mutex> rl(map_mutex);
    if(handler == NULL) return;
    
    uintptr_t handler_key = reinterpret_cast<uintptr_t>(handler);
    if(map_attribute_handler.count(handler_key)) {
        QKC_INFO << "Quantum handler alredy present for key:" << key << " and attribute:" << handler->getAttributeName();
        return;
    }
    map_attribute_handler.insert(std::make_pair(handler_key, handler));
    QKC_INFO << "Added new quantum handler for key:" << key << " and attribute:" << handler->getAttributeName();
}

void QuantumKeyConsumer::removeAttributeHandler(AbstractQuantumKeyAttributeHandler *handler) {
    //aquire write lock to work on map
    boost::unique_lock<boost::mutex> wl(map_mutex);
    if(handler == NULL) return;
    QKC_INFO << "remove quantum handler for key:" << key << " and attribute:" << handler->getAttributeName();
    uintptr_t handler_key = reinterpret_cast<uintptr_t>(handler);
    if(!map_attribute_handler.count(handler_key)) return;
    map_attribute_handler.erase(handler_key);
}

const std::string& QuantumKeyConsumer::getKey() {
    return key;
}

const size_t QuantumKeyConsumer::size() {
    //aquire write lock to work on map
    boost::unique_lock<boost::mutex> wl(map_mutex);
    return map_attribute_handler.size();
}