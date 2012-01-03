//
//  AttributeEventHandlerQueueRunner.h
//  ChaosLib
//
//  Created by Bisegni Claudio on 05/11/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosLib_AttributeEventHandlerQueueRunner_h
#define ChaosLib_AttributeEventHandlerQueueRunner_h


#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/data/CDataWrapper.h>
#include "AttributeEventHandler.h"
namespace chaos {
    using namespace std;
    using namespace boost;
    
    /*
     This class define an environment where handler of attribute set are executed can be executed
     */
    class AttributeEventHandlerQueueRunner : public CObjectProcessingQueue<AbstractAttributeEventHandler> {

    protected:
        /*
         Process the handler
         */
        void processBufferElement(AbstractAttributeEventHandlerPtr) throw(CException);
        
    public:
        /*
         Initialization method for output buffer
         */
        virtual void init(int) throw(CException);
        
        /*
         Deinitialization method for output buffer
         */
        virtual void deinit() throw(CException);
        /*
         
         */
        bool push(AbstractAttributeEventHandlerPtr) throw(CException);
    };
    
    typedef AttributeEventHandlerQueueRunner *AttributeEventHandlerQueueRunnerPtr;
}
#endif
