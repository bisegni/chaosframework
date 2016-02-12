/*
 *	event.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 12/02/16 INFN, National Institute of Nuclear Physics
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
