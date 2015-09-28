/*
 *	ActionDescriptor.cpp
 *	!CHAOS
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
#include <chaos/common/action/ActionDescriptor.h>
using namespace chaos;
using namespace std;
using namespace boost;



AbstractActionDescriptor::AbstractActionDescriptor(){
    fired=false;
    enabled=true;
}

AbstractActionDescriptor::~AbstractActionDescriptor() {
    fired=false;
    enabled=false;
}

/*
 set the string value for the determinated type
 */
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
const string & AbstractActionDescriptor::getTypeValue(ActionStringType sType)  {
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
vector< boost::shared_ptr<ActionParamDescription> >& AbstractActionDescriptor::getParamDescriptions() {
    return paramDescriptionVec;
}

/*
 Add a new param
 */
void AbstractActionDescriptor::addParam(const std::string& param_name,
                                        DataType::DataType type,
                                        const std::string& description){
    boost::shared_ptr<ActionParamDescription> desc (new ActionParamDescription(param_name));
    desc->paramType = type;
    desc->paramDescription = description;
    paramDescriptionVec.push_back(desc);
}
