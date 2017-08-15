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
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/action/DomainActions.h>


using namespace chaos;
using namespace boost;

DomainActions::DomainActions(const string & _domain_name):
domainName(_domain_name) {}

DomainActions::~DomainActions() {    
    //the pointer into the map need to be deleted
    actionDescriptionsMap.clear();
}

/*
 Add an action descriptor to this domain
 */
bool DomainActions::addActionDescriptor(AbstActionDescShrPtr actionDescriptorPtr)  throw(CException){
    //check if the poitner is valid
    if(!actionDescriptorPtr) return false;
    
    //check is the domain is the same
    if((*actionDescriptorPtr).getTypeValue(AbstractActionDescriptor::ActionDomain) != domainName ) return false;
    
    //add the description
    actionDescriptionsMap.insert(make_pair((*actionDescriptorPtr).getTypeValue(AbstractActionDescriptor::ActionName), actionDescriptorPtr));
    
    return true;
}

/*
 remove an action identified by it's descriptor
 */
void DomainActions::removeActionDescriptor(AbstActionDescShrPtr actionDescriptorPtr)  throw(CException){
    //check if the poitner is valid
    if(!actionDescriptorPtr) return;
    
    //check is the domain is the same
    if((*actionDescriptorPtr).getTypeValue(AbstractActionDescriptor::ActionDomain)!=domainName ) return;
    
    //lock the action for write, so we can schedule it
    actionDescriptorPtr->setEnabled(false);

    
    //here need to be made some controll in case that action is running 
    actionDescriptionsMap.erase((*actionDescriptorPtr).getTypeValue(AbstractActionDescriptor::ActionName));
}

/*
 return the domain name
 */
const string& DomainActions::getDomainName() {
    return domainName;
}

uint32_t DomainActions::registeredActions() {
	return (uint32_t)actionDescriptionsMap.size();
}

/*
 check for action name presence
 */
bool DomainActions::hasActionName(const string& actName) {
    return actionDescriptionsMap.count(actName)==1;
}

/*
 return the reference to action desciptor ssociated with action name
 for managed domain
 */
AbstActionDescShrPtr& DomainActions::getActionDescriptornFormActionName(const string& actName) {
    AbstActionDescShrPtr& action = actionDescriptionsMap[actName];
    return action;
}
