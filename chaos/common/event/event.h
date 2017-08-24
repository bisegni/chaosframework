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

#ifndef __CHAOSFramework__event_h
#define __CHAOSFramework__event_h

#include <chaos/common/event/EventHandler.h>
#include <chaos/common/event/EventForwarder.h>
#include <chaos/common/event/AsioEventHandler.h>
#include <chaos/common/event/AsioImplEventServer.h>
#include <chaos/common/event/AsioEventForwarder.h>
#include <chaos/common/event/AsioImplEventClient.h>
#include <chaos/common/event/EventClient.h>
#include <chaos/common/event/EventServer.h>
#include <chaos/common/event/channel/EventChannel.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>
#include <chaos/common/event/channel/AlertEventChannel.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
#include <chaos/common/event/evt_desc/InstrumentEventDescriptor.h>
#include <chaos/common/event/evt_desc/CommandEventDescriptor.h>
#include <chaos/common/event/evt_desc/CustomEventDescriptor.h>
#include <chaos/common/event/evt_desc/EventFactory.h>

#endif /* __CHAOSFramework__event_h */
