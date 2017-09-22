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

#include <chaos/cu_toolkit/control_manager/handler/DatasetAttributeHandler.h>

using namespace chaos::cu::control_manager::handler;

DatasetAttributeHandler::DatasetAttributeHandler() {
    
}

DatasetAttributeHandler::~DatasetAttributeHandler() {
    clearAllAttributeHandler();
}

bool DatasetAttributeHandler::removeHandlerOnAttributeName(const std::string& attribute_name) {
    if(map_handlers_for_attribute.count(attribute_name) == false) return false;
    map_handlers_for_attribute.erase(attribute_name);
    return true;
}

void DatasetAttributeHandler::executeHandlers(chaos::common::data::CDataWrapper *attribute_changes_set) {
    //reset the map fo rthe result by attribute name
    map_handler_result.clear();
    
    //execute all handler storing the result for every one
    for(MapAttributeHandlerIterator it = map_handlers_for_attribute.begin();
        it != map_handlers_for_attribute.end();
        it++) {
        setHandlerResult(it->first, it->second->executeHandler(attribute_changes_set));
    }
}

void DatasetAttributeHandler::clearAllAttributeHandler() {
    map_handlers_for_attribute.clear();
}
