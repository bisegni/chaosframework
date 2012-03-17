/*	
 *	DeclareAction.h
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
#ifndef ChaosFramework_DeclareAction_h
#define ChaosFramework_DeclareAction_h

#include <vector>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
        //!Base class for all other that need to expose an action
    /*!
     This class must manage the action description allocation and disposal
     every class that need to be register into commandMnaager for expose
     action, need to subclass this
     */
    class DeclareAction {
        vector<AbstActionDescShrPtr> actionDescriptionVector;
            //!action
        /*!
         Encode action into his correspondend CDataWrapper
         */
        void decodeAction(AbstActionDescShrPtr&, CDataWrapper&);
    public:
        /*!
         Default destructor
         */
        ~DeclareAction();
        
            //!action definition helper
        /*!
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

        /*!
         return the array that contains all action descriptor for 
         the istance implementing this class
         */
        vector<AbstActionDescShrPtr>& getActionDescriptors();
        
        /*!
         Return the description of all action into a CDataWrapper
         */
        void getActionDescrionsInDataWrapper(CDataWrapper&);
    };
}
#endif
