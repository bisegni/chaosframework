/*
 *	AbstractAttributeHandler.cpp
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

#include <chaos/cu_toolkit/control_manager/handler/AbstractAttributeHandler.h>

using namespace chaos::cu::control_manager::handler;

AbstractAttributeHandler::AbstractAttributeHandler() {}
AbstractAttributeHandler::~AbstractAttributeHandler() {}

void AbstractAttributeHandler::setHandlerResult(const std::string& attribute_name,
                                                const bool result) {
    map_handler_result[attribute_name] = result;
}

bool AbstractAttributeHandler::getHandlerResult(const std::string& attirbute_name) {
    //if we have no registered handle for that attribute return true(the handler can acept or no the change on the attribute)
    if(map_handler_result.count(attirbute_name) == 0) return true;
    return map_handler_result[attirbute_name];
}