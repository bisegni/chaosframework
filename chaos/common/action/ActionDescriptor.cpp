//
//  ActionDescriptor.cpp
//  ChaosFramework
//
//  Created by bisegni on 11/07/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include "ActionDescriptor.h"
using namespace chaos;
using namespace std;
using namespace boost;


AbstractActionDescriptor::AbstractActionDescriptor(){
    fired=false;
    enabled=true;
}

AbstractActionDescriptor::~AbstractActionDescriptor() {
    
}

/*
 set the string value for the determinated type
 */
void AbstractActionDescriptor::setTypeValue(ActionStringType sType, string& sValue) {
    setTypeValue(sType, sValue.c_str());
}

/*
 set the string value for the determinated type
 */
void AbstractActionDescriptor::setTypeValue(ActionStringType sType, const char*const sValue) {
    switch (sType) {
        case ActionDomain:
            actionDomain.assign(sValue);
            break;
        case ActionName:
            actionName.assign(sValue);
            break;
            
        case ActionDescription:
            actionDescription.assign(sValue);
            break;
    }
}

bool AbstractActionDescriptor::isFired(){
    ActionReadLock lockAction(actionAccessMutext);
    return fired;
}

bool AbstractActionDescriptor::setFiredWriteLocked(bool _fired){
    ActionWriteLock lockAction(actionAccessMutext);
    //return true only if  fired and enabled are true
    return setFired(_fired);
}

bool AbstractActionDescriptor::setFired(bool _fired){
    fired =_fired && enabled;
    return fired;
}

bool AbstractActionDescriptor::isEnabled(){
    ActionReadLock lockAction(actionAccessMutext);
    return enabled;
}

bool AbstractActionDescriptor::setEnabled(bool _enabled){
    ActionWriteLock lockAction(actionAccessMutext);
    if(_enabled && fired) {
        //attent the termination
    }
    return enabled =_enabled;
}

/*
 get the string value for the determinated type, a reference
 has been return so keep in mind that string live within object life
 */        
string& AbstractActionDescriptor::getTypeValue(ActionStringType sType)  {
    if (sType == ActionDomain) {
        return actionDomain;
    } else if (sType == ActionName) {
        return actionName;
    } else return actionDescription;
}

#pragma mark Param Method
/*
 Return the array list of the param defined by this action
 */
vector< shared_ptr<ActionParamDescription> >& AbstractActionDescriptor::getParamDescriptions() {
    return paramDescriptionVec;
}

/*
 Add a new param
 */
void AbstractActionDescriptor::addParam(const char*const paramName, DataType::DataType type, const char*const description){
    shared_ptr<ActionParamDescription> desc (new ActionParamDescription(paramName));
    desc->paramType = type;
    desc->paramDescription = description;
    paramDescriptionVec.push_back(desc);
}
