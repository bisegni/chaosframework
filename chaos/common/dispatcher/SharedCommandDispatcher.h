/*
 *	SharedCommandDispatcher.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 05/01/2017 INFN, National Institute of Nuclear Physics
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
                          AbstractCommandDispatcher),
    private CObjectProcessingQueue<chaos::common::data::CDataWrapper> {
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
