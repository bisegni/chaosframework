    //
    //  EventBroker.h
    //  CHAOSFramework
    //
    //  Created by Claudio Bisegni on 3/22/12.
    //  Copyright (c) 2012 INFN. All rights reserved.
    //

#ifndef CHAOSFramework_EventBroker_h
#define CHAOSFramework_EventBroker_h

#include <chaos/common/exception/CException.h>

namespace chaos {
    
    
#define EVT_HB          0
#define EVT_DEV_OFFLINE 1
    
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
    
    
    class EventBroker {
        
        
    public:
            //! Basic Constructor
        EventBroker();
        
            //! Basic Destructor
        ~EventBroker();
        
            //!Event Broker initialization
        /*!
         * Initzialize the Event Broker. In this step are taken the configured implementation
         * for the rpc client and server and for the dispatcher. All these are here initialized
         */
        void init() throw(CException);
        
            //!EventBroker deinitialization
        /*!
         * All rpc adapter and command dispatcher are deinitilized. All instantiated channel are disposed
         */
        void deinit() throw(CException);
        
            //!start all internal engine for client, server and event dispatcher
        /*!
         * all part are started
         */
        void start() throw(CException);
    };
    
}
#endif
