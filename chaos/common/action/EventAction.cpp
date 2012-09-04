/*
 *	EventAction.cpp
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 29/08/12.
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
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/action/EventAction.h>

using namespace std;
using namespace chaos;
using namespace boost;

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
