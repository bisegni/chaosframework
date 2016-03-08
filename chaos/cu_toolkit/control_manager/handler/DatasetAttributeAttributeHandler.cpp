/*
 *	DatasetAttributeAttributeHandler.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 08/03/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/control_manager/handler/DatasetAttributeAttributeHandler.h>

using namespace chaos::cu::control_manager::handler;

DatasetAttributeAttributeHandler::DatasetAttributeAttributeHandler() {
    
}

DatasetAttributeAttributeHandler::~DatasetAttributeAttributeHandler() {
    
}

bool DatasetAttributeAttributeHandler::removeHandlerOnAttributeName(const std::string& attribute_name) {
    if(map_handlers_for_attribute.count(attribute_name) == false) return false;
    map_handlers_for_attribute.erase(attribute_name);
}

void DatasetAttributeAttributeHandler::executeHandlers(chaos::common::data::CDataWrapper *attribute_changes_set) {
    //reset the map fo rthe result by attribute name
    map_handler_result.clear();
    
    //execute all handler storing the result for every one
    for(MapAttributeHandlerIterator it = map_handlers_for_attribute.begin();
        it != map_handlers_for_attribute.end();
        it++) {
        setHandlerResult(it->first, it->second->executeHandler(attribute_changes_set));
    }
}