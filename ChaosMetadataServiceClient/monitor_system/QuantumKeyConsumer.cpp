/*
 *	QuantumTypedConsumer.cpp
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
    boost::shared_lock<boost::shared_mutex> rl(map_mutex);
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
    boost::shared_lock<boost::shared_mutex> rl(map_mutex);
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
    boost::unique_lock<boost::shared_mutex> rl(map_mutex);
    if(handler == NULL) return;
    
    QKC_INFO << "Add new quantum consumer for key:" << key << " and attribute:" << handler->getAttributeName();
    uintptr_t handler_key = reinterpret_cast<uintptr_t>(handler);
    if(map_attribute_handler.count(handler_key)) return;
    map_attribute_handler.insert(make_pair(handler_key, handler));
}

void QuantumKeyConsumer::removeAttributeHandler(AbstractQuantumKeyAttributeHandler *handler) {
    //aquire write lock to work on map
    boost::unique_lock<boost::shared_mutex> wl(map_mutex);
    if(handler == NULL) return;

    QKC_INFO << "remove quantum consumer for key:" << key << " and attribute:" << handler->getAttributeName();
    uintptr_t handler_key = reinterpret_cast<uintptr_t>(handler);
    if(!map_attribute_handler.count(handler_key)) return;
    map_attribute_handler.erase(handler_key);
}

const std::string& QuantumKeyConsumer::getKey() {
    return key;
}

const size_t QuantumKeyConsumer::size() {
    //aquire write lock to work on map
    boost::shared_lock<boost::shared_mutex> rl(map_mutex);
    return map_attribute_handler.size();
}