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
#ifndef ChaosFramework_DeclareAction_h
#define ChaosFramework_DeclareAction_h

#include <string>
#include <vector>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    
    namespace chaos_data = chaos::common::data;
    
    //!Base class for all other that need to expose an action
    /*!
     This class must manage the action description allocation and disposal
     every class that need to be register into commandMnaager for expose
     action, need to subclass this
     */
    class DeclareAction {
        mutable std::vector<AbstActionDescShrPtr> actionDescriptionVector;
        //!action
        /*!
         Encode action into his correspondend CDataWrapper
         */
        void decodeAction(AbstActionDescShrPtr&, chaos_data::CDataWrapper&) const;
    public:
        DeclareAction();
        /*!
         Default destructor
         */
        virtual ~DeclareAction();
        
        //!action definition helper
        /*!
         Create a new action description, returning it's shared pointer that can be used to configura the parametter
         */
        template<typename T>
        AbstActionDescShrPtr addActionDescritionInstance(T* actonObjectPointer,
                                                         typename ActionDescriptor<T>::ActionPointerDef actionHandler,
                                                         const std::string& actionDomainName,
                                                         const std::string& actionAliasName,
                                                         const std::string& actionDescription,
                                                         bool shared_execution = false) {
            AbstActionDescShrPtr new_action;
            try{
                new_action.reset(new ActionDescriptor<T>(actonObjectPointer,
                                                                           actionHandler,
                                                                           actionDomainName,
                                                                           actionAliasName,
                                                                           shared_execution));
                new_action->setTypeValue(ActionDescriptor<T>::ActionDescription, actionDescription);
                //add action description to vector
                actionDescriptionVector.push_back(new_action);
            } catch (...) {
                throw CException(-1, "Error adding new action", __PRETTY_FUNCTION__);
            }
            return new_action;
        }
        
        /*!
         return the array that contains all action descriptor for
         the istance implementing this class
         */
        std::vector<AbstActionDescShrPtr>& getActionDescriptors();
        
        /*!
         Return the description of all action into a CDataWrapper
         */
        void getActionDescrionsInDataWrapper(chaos::common::data::CDataWrapper&, bool close = true) const;
    };
}
#endif
