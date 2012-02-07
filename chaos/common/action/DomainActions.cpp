//
//  DomainActions.cpp
//  ChaosFramework
//
//  Created by bisegni on 07/07/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include "../global.h"
#include "DomainActions.h"
#include "../cconstants.h"

using namespace chaos;
using namespace boost;

DomainActions::DomainActions(const char*const _domainName) {
    domainName.assign(_domainName);
}

DomainActions::DomainActions(string& _domainName) {
    domainName.assign(_domainName.c_str());
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