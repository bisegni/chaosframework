/*
 *	EventClient.cpp
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 25/08/12.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/event/EventClient.h>

using namespace chaos;
using namespace chaos::event;

EventClient::EventClient(string *alias): NamedService(alias) {
}

/*
 init the event adapter
 */
void EventClient::init(void*) throw(CException) {
}

/*
 start the event adapter
 */
void EventClient::start() throw(CException) {
    
}

/*
 deinit the event adapter
 */
void EventClient::deinit() throw(CException) {
    LAPP_ << "Deinitializing Domain Actions Scheduler for domain:";
        //mutex::scoped_lock lockAction(actionAccessMutext);
}

void EventClient::setThreadNumber(unsigned int newThreadNumber) {
    threadNumber = newThreadNumber;
}

unsigned int EventClient::getThreadNumber() {
    return threadNumber;
}