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
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/action/EventAction.h>

using namespace std;
using namespace boost;
using namespace chaos;
using namespace chaos::common::utility;

EventAction::EventAction() {
        //create the unique uuid for action, the uuid lite version can work in this context
    actionUUID = UUIDUtil::generateUUIDLite();
}

EventAction::~EventAction() {
    
}

void EventAction::setDomainName(string& newDomainName) {
    domainName = newDomainName;
}

const char * const EventAction::getDomainName() {
    return domainName.c_str();
}

/*
 Return c string of the domain name
 */
const char * const EventAction::getUUID() {
    return actionUUID.c_str();
}

/*
 */
void EventAction::registerForIdentifier(const char * newIdentifier) {
    boost::shared_lock< boost::shared_mutex >   lock(identifierMutext);
    indetifierExecutioTrigger.insert(make_pair(newIdentifier, true));
}

/*
 */
void EventAction::deregisterForIdentifier(const char * newIdentifier) {
    boost::shared_lock< boost::shared_mutex >   lock(identifierMutext);
    if(indetifierExecutioTrigger.count(newIdentifier) != 0) return;
    
    indetifierExecutioTrigger.erase(newIdentifier);
}
/*
 */
void EventAction::clearAllRegisteredIdentifier(const char * newIdentifier) {
     boost::shared_lock< boost::shared_mutex >   lock(identifierMutext);
    indetifierExecutioTrigger.clear();
}

/*
 */
bool EventAction::hasIdentifier(const char * newIdentifier) {
    boost::shared_lock< boost::shared_mutex >   lock(identifierMutext);
    bool result = (indetifierExecutioTrigger.count(newIdentifier) > 0) || indetifierExecutioTrigger.size()==0;
    return result;
}
