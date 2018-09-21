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

#include <chaos/common/event/EventClient.h>

using namespace std;
using namespace chaos;
using namespace chaos::common::event;

EventClient::EventClient(string alias):
NamedService(alias),
threadNumber(0){}

/*
 init the event adapter
 */
void EventClient::init(void*) {
}

/*
 start the event adapter
 */
void EventClient::start() {
    
}

/*
 deinit the event adapter
 */
void EventClient::deinit() {
    LAPP_ << "Deinitializing Domain Actions Scheduler for domain:";
        //mutex::scoped_lock lockAction(actionAccessMutext);
}

void EventClient::setThreadNumber(unsigned int newThreadNumber) {
    threadNumber = newThreadNumber;
}

unsigned int EventClient::getThreadNumber() {
    return threadNumber;
}
