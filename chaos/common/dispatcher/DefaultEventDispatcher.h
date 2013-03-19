/*
 *	DefaultEventDispatcher.h
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

#ifndef CHAOSFramework_DefaultEventDispatcher_h
#define CHAOSFramework_DefaultEventDispatcher_h
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/dispatcher/AbstractEventDispatcher.h>
#include <boost/thread.hpp>

namespace chaos {
    using namespace event;
    using namespace std;
    
    class EventTypeScheduler;
    
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(DefaultEventDispatcher, AbstractEventDispatcher) {
        
            //! Alert scheduler
        EventTypeScheduler *alertEventScheduler;

            //! Alert scheduler
        EventTypeScheduler *instrumentEventScheduler;
        
        boost::shared_mutex handlerVEctorMutext;
    protected:
        //!Event dispatcher initialization
        /*!
         * Initzialize the Event Broker
         */
        void init(void *initData) throw(CException);
        
            //!Event dispatcher initialization
        /*!
         * Initzialize the Event Broker
         */
        void start() throw(CException);
        
        //-----------------------
        void stop() throw(CException){};
        
            //!Event dispatcher deinitialization
        /*!
         * All resource aredeinitialized
         */
        void deinit() throw(CException);
        
        /*!
         Thsi is the methdo that is called when the specified event is recognized
         by dispatcher
         */
        virtual void executeAlertHandler(alert::AlertEventDescriptor *eventDescription)  throw(CException);
            //!Handler execution method
        /*!
         Thsi is the methdo that is called when the specified event is recognized
         by dispatcher
         */
        virtual void executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription)  throw(CException);
            //!Handler execution method
        /*!
         Thsi is the methdo that is called when the specified event is recognized
         by dispatcher
         */
        virtual void executeCommandHandler(command::CommandEventDescriptor *eventDescription)  throw(CException);
            //!Handler execution method
        /*!
         Thsi is the methdo that is called when the specified event is recognized
         by dispatcher
         */
        virtual void executeCustomHandler(custom::CustomEventDescriptor* eventDescription)  throw(CException);
        
    public:
            //! Basic Constructor
        DefaultEventDispatcher(string *alias);
        
            //! Basic Destructor
        virtual ~DefaultEventDispatcher();
        
            //! Event handler registration
        /*
         Perform the registration of an handler
         */
        void registerEventAction(EventAction *eventAction, EventType eventType, const char * const identificationString = NULL)  throw(CException);
        
            //! Event handler deregistration
        /*
         Perform the deregistration of an handler
         */
        void deregisterEventAction(EventAction *eventAction)  throw(CException);


    };
    
}
#endif
