/*
 *	DefaultWANInterfaceHandler.cpp
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
#include "DefaultWANInterfaceHandler.h"

#include "api/producer/ProducerGroup.h"

using namespace chaos::wan_proxy;

DefaultWANInterfaceHandler::DefaultWANInterfaceHandler(persistence::AbstractPersistenceDriver *_persistence_driver):
BaseWANInterfacelHandler(_persistence_driver){
}

DefaultWANInterfaceHandler::~DefaultWANInterfaceHandler() {
    
}

// register the group defined by the handler
void DefaultWANInterfaceHandler::registerGroup() {
	//add group to the version 1
	addGroupToVersion<api::producer::ProducerGroup>(1);
}