/*
 *	AbstractEventHandler.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 16/02/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/event/AbstractEventHandler.h>

using namespace chaos::common::event;

using namespace chaos::metadata_service_client::event;


AbstractEventHandler::AbstractEventHandler(EventType _type,
                                           const unsigned int _subtype):
type(_type),
subtype(_subtype){}

AbstractEventHandler::~AbstractEventHandler() {}

const EventType& AbstractEventHandler::getType() {
    return type;
}

const unsigned int AbstractEventHandler::getSubType() {
    return subtype;
}