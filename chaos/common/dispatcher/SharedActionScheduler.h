/*
 *	SharedActionScheduler.h
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

#ifndef __CHAOSFramework__9C37EF8_1E1D_4E93_84A4_9BA3C3F3AEA7_SharedActionScheduler_h
#define __CHAOSFramework__9C37EF8_1E1D_4E93_84A4_9BA3C3F3AEA7_SharedActionScheduler_h

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/dispatcher/AbstractActionScheduler.h>

namespace chaos {
    
    CHAOS_DEFINE_MAP_FOR_TYPE(std::string, boost::shared_ptr<DomainActions>, MapDomainActions);
    
    CHAOS_DEFINE_LOCKABLE_OBJECT(MapDomainActions, MapDomainActionsLocked);
    
    //! class scheduler for executing the rpc action belong to multiple domain
    /*!
     This scheduler mantins the queue for every domains and share dthe thread for executions of
     every action
     */
    class SharedActionScheduler:
    public AbstractActionScheduler,
    private CObjectProcessingQueue<chaos_data::CDataWrapper>{
        
        MapDomainActionsLocked map_domain_actions;
    protected:
        virtual void processBufferElement(chaos_data::CDataWrapper*,
                                          ElementManagingPolicy&) throw(CException);
    public:
        /*!
         Default constructor
         */
        SharedActionScheduler();
        
        /*!
         Default constructor
         */
        virtual ~SharedActionScheduler();
        
        void addActionDomain(boost::shared_ptr<DomainActions> new_action_domain);
        
        void removeActionDomain(boost::shared_ptr<DomainActions> new_action_domain);
        
        /*!
         Initialization method
         */
        virtual void init(int) throw(CException);
        
        /*!
         Deinitialization method
         */
        virtual void deinit() throw(CException);
        /*!
         Push a new action pack into the queue
         */
        bool push(chaos_data::CDataWrapper *action_submission_pack) throw(CException);
        
        
        //!call the action in an async way
        virtual void synchronousCall(chaos_data::CDataWrapper *message,
                                     chaos_data::CDataWrapper *result);
        
        //! return the number of queued action
        uint32_t getQueuedActionSize();
    };
    
}

#endif /* __CHAOSFramework__9C37EF8_1E1D_4E93_84A4_9BA3C3F3AEA7_SharedActionScheduler_h */
