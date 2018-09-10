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

#ifndef CHAOSFramework_DefaultEventDispatcher_h
#define CHAOSFramework_DefaultEventDispatcher_h
//#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/dispatcher/AbstractEventDispatcher.h>
#include <boost/thread.hpp>

namespace chaos {
    using namespace common::event;
    using namespace std;
    
    class EventTypeScheduler;
    
    DECLARE_CLASS_FACTORY(DefaultEventDispatcher, AbstractEventDispatcher) {
        
            //! Alert scheduler
        EventTypeScheduler *eventScheduler;
        
        boost::shared_mutex handlerVEctorMutext;
    protected:
        //!Event dispatcher initialization
        /*!
         * Initzialize the Event Broker
         */
        void init(void *initData);
        
            //!Event dispatcher initialization
        /*!
         * Initzialize the Event Broker
         */
        void start();
        
        //-----------------------
        void stop(){};
        
            //!Event dispatcher deinitialization
        /*!
         * All resource aredeinitialized
         */
        void deinit();
        
        /*!
         Thsi is the methdo that is called when the specified event is recognized
         by dispatcher
         */
        virtual void executeAlertHandler(alert::AlertEventDescriptor *eventDescription) ;
            //!Handler execution method
        /*!
         Thsi is the methdo that is called when the specified event is recognized
         by dispatcher
         */
        virtual void executeInstrumentHandler(instrument::InstrumentEventDescriptor *eventDescription) ;
            //!Handler execution method
        /*!
         Thsi is the methdo that is called when the specified event is recognized
         by dispatcher
         */
        virtual void executeCommandHandler(command::CommandEventDescriptor *eventDescription) ;
            //!Handler execution method
        /*!
         Thsi is the methdo that is called when the specified event is recognized
         by dispatcher
         */
        virtual void executeCustomHandler(custom::CustomEventDescriptor* eventDescription) ;
        
    public:
            //! Basic Constructor
        DefaultEventDispatcher(string alias);
        
            //! Basic Destructor
        virtual ~DefaultEventDispatcher();
        
            //! Event handler registration
        /*
         Perform the registration of an handler
         */
        void registerEventAction(EventAction *eventAction, EventType eventType, const char * const identificationString = NULL) ;
        
            //! Event handler deregistration
        /*
         Perform the deregistration of an handler
         */
        void deregisterEventAction(EventAction *eventAction) ;


    };
    
}
#endif
