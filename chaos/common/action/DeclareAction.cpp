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

#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/chaos_constants.h>

using namespace chaos;
using namespace boost;
namespace chaos_data = chaos::common::data;

DeclareAction::~DeclareAction() {
        //dispose all descriptors
    actionDescriptionVector.clear();
}

std::vector<AbstActionDescShrPtr>& DeclareAction::getActionDescriptors() {
    return actionDescriptionVector;
}


/*
 Return the description of all action into a CDataWrapper
 */
void DeclareAction::getActionDescrionsInDataWrapper(chaos_data::CDataWrapper& actionsDescription, bool close) const {
    ChaosSharedPtr<chaos_data::CDataWrapper> actionDescription;
    std::vector<AbstActionDescShrPtr>::iterator actionIter;
    
        //cycle all actions for construct the vector of param action
    for (actionIter = actionDescriptionVector.begin(); 
         actionIter != actionDescriptionVector.end(); 
         actionIter++) {
            //decode action into CDataWrapper
        chaos_data::CDataWrapper actionDescription;
            //fill description with action value
        decodeAction(*actionIter, actionDescription);
            //add description to array of action for this declaring class
        actionsDescription.appendCDataWrapperToArray(actionDescription);
    }
    
        //finalize the vector with the appropiate key
    if(close)actionsDescription.finalizeArrayForKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DESC);
}

/*
 Return the description of all action into a CDataWrapper
 */
void DeclareAction::decodeAction(AbstActionDescShrPtr& actionDesc, chaos_data::CDataWrapper& actionDescription) const {
        //add domain for the action 
    actionDescription.addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN, actionDesc->getTypeValue(AbstractActionDescriptor::ActionDomain));
    
        //add name for the action
    actionDescription.addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME, actionDesc->getTypeValue(AbstractActionDescriptor::ActionName));
    
        //add the information for the action
    actionDescription.addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DESCRIPTION, actionDesc->getTypeValue(AbstractActionDescriptor::ActionDescription));
    
        //now i must describe the param for this action
    std::vector< ChaosSharedPtr<ActionParamDescription> >& paramDescriptionVector = actionDesc->getParamDescriptions();
    
    if(paramDescriptionVector.size()){
            //there are some parameter for this action, need to be added to rapresentation
        ChaosSharedPtr<chaos_data::CDataWrapper> paramDescRepresentation(new chaos_data::CDataWrapper());
        for (std::vector< ChaosSharedPtr<ActionParamDescription> >::iterator paramIter = paramDescriptionVector.begin();
             paramIter !=paramDescriptionVector.end();
             paramIter++) {
            
                //add thename of the parameter
            paramDescRepresentation->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_NAME, (*paramIter)->paramName);
            
                //add the information about the parameter
            paramDescRepresentation->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_INFO, (*paramIter)->paramDescription);
            
                //add the parameter type
            paramDescRepresentation->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_TYPE, (*paramIter)->paramType);

                // add parametere representation object to main action representation
            actionDescription.appendCDataWrapperToArray(*paramDescRepresentation.get());
        }
        
            //cloese the array
        actionDescription.finalizeArrayForKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PARAM);
    }
}
