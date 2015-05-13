//
//  QuantumTypedConsumer.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 13/05/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include "QuantumKeyConsumer.h"

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
        if(value->hasKey(it->first)){
            //broadcast value
            it->second->_consumeValue(key,
                                      value);
        }
    }
}

void QuantumKeyConsumer::addAttributeHandler(AbstractQuantumKeyAttributeHandler *handler) {
    //aquire write lock to work on map
    boost::unique_lock<boost::shared_mutex> rl(map_mutex);
    if(map_attribute_handler.count(handler->attribute)) return;
    map_attribute_handler.insert(make_pair(handler->attribute, handler));
}

void QuantumKeyConsumer::removeAttributeHandler(AbstractQuantumKeyAttributeHandler *handler) {
    //aquire write lock to work on map
    boost::unique_lock<boost::shared_mutex> wl(map_mutex);
    if(!map_attribute_handler.count(handler->attribute)) return;
    map_attribute_handler.erase(handler->attribute);
}

const std::string& QuantumKeyConsumer::getKey() {
    return key;
}

const size_t QuantumKeyConsumer::size() {
    //aquire write lock to work on map
    boost::shared_lock<boost::shared_mutex> rl(map_mutex);
    return map_attribute_handler.size();
}