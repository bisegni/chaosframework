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
#include "DefaultWANInterfaceHandler.h"

#include "api/producer/ProducerGroup.h"
#include "api/consumer/ConsumerGroup.h"

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
	addGroupToVersion<api::consumer::ConsumerGroup>(1);
}
