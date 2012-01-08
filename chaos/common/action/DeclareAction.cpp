    //
    //  DeclareAction.cpp
    //  ChaosFramework
    //
    //  Created by bisegni on 07/07/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include "DeclareAction.h"
#include "../cconstants.h"

using namespace chaos;

DeclareAction::~DeclareAction() {
        //dispose all descriptors
    actionDescriptionVector.clear();
}

vector<AbstActionDescShrPtr>& DeclareAction::getActionDescriptors() {
    return actionDescriptionVector;
}


/*
 Return the description of all action into a CDataWrapper
 */
void DeclareAction::getActionDescrionsInDataWrapper(CDataWrapper& actionsDescription) {
    shared_ptr<CDataWrapper> actionDescription;
    vector<AbstActionDescShrPtr>::iterator actionIter;
    
        //cycle all actions for construct the vector of param action
    for (actionIter = actionDescriptionVector.begin(); 
         actionIter != actionDescriptionVector.end(); 
         actionIter++) {
            //decode action into CDataWrapper
        CDataWrapper actionDescription;
            //fill description with action value
        decodeAction(*actionIter, actionDescription);
            //add description to array of action for this declaring class
        actionsDescription.appendCDataWrapperToArray(actionDescription);
    }
    
        //finalize the vector with the appropiate key
    actionsDescription.finalizeArrayForKey(CommandManagerConstant::CS_CMDM_ACTION_DESC);
}

/*
 Return the description of all action into a CDataWrapper
 */
void DeclareAction::decodeAction(AbstActionDescShrPtr& actionDesc, CDataWrapper& actionDescription) {
        //add domain for the action 
    actionDescription.addStringValue(CommandManagerConstant::CS_CMDM_ACTION_DOMAIN, actionDesc->getTypeValue(AbstractActionDescriptor::ActionDomain));
    
        //add name for the action
    actionDescription.addStringValue(CommandManagerConstant::CS_CMDM_ACTION_NAME, actionDesc->getTypeValue(AbstractActionDescriptor::ActionName));
    
        //add the information for the action
    actionDescription.addStringValue(CommandManagerConstant::CS_CMDM_ACTION_DESCRIPTION, actionDesc->getTypeValue(AbstractActionDescriptor::ActionDescription));
    
        //now i must describe the param for this action
    vector< shared_ptr<ActionParamDescription> >& paramDescriptionVector = actionDesc->getParamDescriptions();
    
    if(paramDescriptionVector.size()){
            //there are some parameter for this action, need to be added to rapresentation
        shared_ptr<CDataWrapper> paramDescRepresentation(new CDataWrapper());
        for (vector< shared_ptr<ActionParamDescription> >::iterator paramIter = paramDescriptionVector.begin();
             paramIter !=paramDescriptionVector.end();
             paramIter++) {
            
                //add thename of the parameter
            paramDescRepresentation->addStringValue(CommandManagerConstant::CS_CMDM_ACTION_DESC_PAR_NAME, (*paramIter)->paramName);
            
                //add the information about the parameter
            paramDescRepresentation->addStringValue(CommandManagerConstant::CS_CMDM_ACTION_DESC_PAR_INFO, (*paramIter)->paramDescription);
            
                //add the parameter type
            paramDescRepresentation->addInt32Value(CommandManagerConstant::CS_CMDM_ACTION_DESC_PAR_TYPE, (*paramIter)->paramType);

                // add parametere representation object to main action representation
            actionDescription.appendCDataWrapperToArray(*paramDescRepresentation.get());
        }
        
            //cloese the array
        actionDescription.finalizeArrayForKey(CommandManagerConstant::CS_CMDM_ACTION_DESC_PARAM);
    }
    
    
}