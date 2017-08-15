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

#ifndef __CHAOSFramework__525A258_D170_4D87_ADFE_26B6B36E6881_SharedCommandDispatcher_h
#define __CHAOSFramework__525A258_D170_4D87_ADFE_26B6B36E6881_SharedCommandDispatcher_h


#include <string>
#include <boost/shared_ptr.hpp>

#include <chaos/common/chaos_types.h>
#include <chaos/common/thread/ChaosThread.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/dispatcher/DomainActionsScheduler.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/LockableObject.h>

namespace chaos{
    
    CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr<DomainActions>, MapDomainActions);
    
    CHAOS_DEFINE_LOCKABLE_OBJECT(MapDomainActions, MapDomainActionsLocked);
    
    /*!
     This class implemente the shared execution environment for pc action
     */
    DECLARE_CLASS_FACTORY(SharedCommandDispatcher,
                          AbstractCommandDispatcher){
        MapDomainActionsLocked map_domain_actions;
    protected:
        void processBufferElement(chaos_data::CDataWrapper*,
                                  ElementManagingPolicy&) throw(CException);
    public:
        SharedCommandDispatcher(const string& alias);
        virtual ~SharedCommandDispatcher();
        
        /*!
         Initialization method for output buffer
         */
        virtual void init(void *) throw(CException);
        
        /*!
         Deinitialization method for output buffer
         */
        virtual void deinit() throw(CException);
        
        /*!
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        virtual void registerAction(DeclareAction*)  throw(CException) ;
        
        /*!
         Deregister actions for a determianted domain
         */
        virtual void deregisterAction(DeclareAction*)  throw(CException) ;
        
        // inherited method
        chaos::common::data::CDataWrapper* dispatchCommand(chaos::common::data::CDataWrapper*) throw(CException);
        
        // inherited method
        chaos::common::data::CDataWrapper* executeCommandSync(chaos::common::data::CDataWrapper * action_pack);
        
        //inherited method
        uint32_t domainRPCActionQueued(const std::string& domain_name);
        
        bool hasDomain(const std::string& domain_name);
    };
}


#endif /* __CHAOSFramework__525A258_D170_4D87_ADFE_26B6B36E6881_SharedCommandDispatcher_h */
