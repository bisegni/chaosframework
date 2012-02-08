//
//  DeclareAction.h
//  ChaosFramework
//
//  Created by bisegni on 07/07/11.
//  Copyright 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_DeclareAction_h
#define ChaosFramework_DeclareAction_h

#include <vector>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    /*
     This class must manage the action description allocation and disposal
     every class that need to be register into commandMnaager for expose
     action, need to subclass this
     */
    class DeclareAction {
        vector<AbstActionDescShrPtr> actionDescriptionVector;
        
        /*
         Decode one action to his correspondend CDataWrapper
         */
        void decodeAction(AbstActionDescShrPtr&, CDataWrapper&);
    public:
        ~DeclareAction();
        /*
         Create a new action description, returning it's shared pointer that can be used to configura the parametter
         */
        template<typename T>
        AbstActionDescShrPtr addActionDescritionInstance(T* actonObjectPointer, typename ActionDescriptor<T>::ActionPointerDef actionHandler, const char*const actionDomainName, const char*const actionAliasName, const char*const actionDescription) {
            
            
            AbstActionDescShrPtr newActionDesc(new ActionDescriptor<T>(actonObjectPointer, actionHandler, actionDomainName, actionAliasName));
            newActionDesc->setTypeValue(ActionDescriptor<T>::ActionDescription, actionDescription);
            //add action description to vector
            actionDescriptionVector.push_back(newActionDesc);
            
            return newActionDesc;
        }

        /*
         return the array that contains all action descriptor for 
         the istance implementing this class
         */
        vector<AbstActionDescShrPtr>& getActionDescriptors();
        
        /*
         Return the description of all action into a CDataWrapper
         */
        void getActionDescrionsInDataWrapper(CDataWrapper&);
    };
}

#endif
