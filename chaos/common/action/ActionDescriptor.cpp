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
#include <chaos/common/action/ActionDescriptor.h>
using namespace chaos;
using namespace boost;
using namespace std;

AbstractActionDescriptor::AbstractActionDescriptor(bool shared_execution):
fired(false),
enabled(true),
shared_execution(shared_execution){}

AbstractActionDescriptor::~AbstractActionDescriptor() {}

void AbstractActionDescriptor::setTypeValue(ActionStringType sType, const string & sValue) {
    switch (sType) {
        case ActionDomain:
            actionDomain = sValue;
            break;
        case ActionName:
            actionName = sValue;
            break;
            
        case ActionDescription:
            actionDescription = sValue;
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

bool AbstractActionDescriptor::isShared() {
    return shared_execution;
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

const string & AbstractActionDescriptor::getTypeValue(ActionStringType sType)  {
    if (sType == ActionDomain) {
        return actionDomain;
    } else if (sType == ActionName) {
        return actionName;
    } else return actionDescription;
}

#pragma mark Param Method
vector< ChaosSharedPtr<ActionParamDescription> >& AbstractActionDescriptor::getParamDescriptions() {
    return paramDescriptionVec;
}

void AbstractActionDescriptor::addParam(const std::string& param_name,
                                        DataType::DataType type,
                                        const std::string& description){
    ChaosSharedPtr<ActionParamDescription> desc (new ActionParamDescription(param_name));
    desc->paramType = type;
    desc->paramDescription = description;
    paramDescriptionVec.push_back(desc);
}
