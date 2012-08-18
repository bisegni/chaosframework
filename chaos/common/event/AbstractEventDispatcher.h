/*
 *	AbstractEventDispatcher.h
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

#ifndef CHAOSFramework_AbstractEventDispatcher_h
#define CHAOSFramework_AbstractEventDispatcher_h

#include <chaos/common/event/EventServerHandler.h>

namespace chaos {
    
        //!Event Broker managment class
    /*!
     This is the main class that manage the vent broadcasting from an internal chaos framework class and a remote chaos endpoint. 
     Chaos event is base on multicast message for sharing event from different broker instance. The event are based on type of the event
     and the data for a determinate event. A basic type are provided, other type can be customized. The heartbeat format is:
     
     32bit fro type, 
     
     The defined event type are:
     
     type EVT_HB, this type represent the event for a notification heratbeat message and the value is the deviceID sent from that device
     type EVT_DEV_OFFLINE, this type represent the event for an offline notification for a determinated device, the value is the deviceID for that device
     */
    
    
    class AbstractEventDispatcher : public EventServerHanlder {
       
    protected:
        /*!
         * Initzialize the Event Broker. In this step are taken the configured implementation
         * for the rpc client and server and for the dispatcher. All these are here initialized
         */
        void init() throw(CException);
        
            //!AbstractEventDispatcher deinitialization
        /*!
         * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
         */
        void deinit() throw(CException);
        
            //!start all internal engine for client, server and event dispatcher
        /*!
         * all part are started
         */
        void start() throw(CException);
        
    public:
            //! Basic Constructor
        AbstractEventDispatcher();
        
            //! Basic Destructor
        ~AbstractEventDispatcher();
        
           
        void dispatchEvent(EventDescriptor*)  throw(CException);

    };
    
}
#endif
