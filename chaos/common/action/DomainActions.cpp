/*	
 *	DomainActions.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio. 
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
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/action/DomainActions.h>


using namespace chaos;
using namespace boost;

DomainActions::DomainActions(const string & _domain_name):domainName(_domain_name) {
}

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
string& DomainActions::getDomainName() {
    return domainName;
}

uint32_t DomainActions::registeredActions() {
	return (uint32_t)actionDescriptionsMap.size();
}

/*
 check for action name presence
 */
bool DomainActions::hasActionName(string& actName) {
    return actionDescriptionsMap.count(actName)==1;
}

/*
 return the reference to action desciptor ssociated with action name
 for managed domain
 */
AbstActionDescShrPtr& DomainActions::getActionDescriptornFormActionName(string& actName) {
    AbstActionDescShrPtr& action = actionDescriptionsMap[actName];
    return action;
}
