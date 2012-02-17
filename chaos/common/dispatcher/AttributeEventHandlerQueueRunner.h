/*	
 *	AttributeEventHandlerQueueRunner.h
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
#ifndef ChaosFramework_AttributeEventHandlerQueueRunner_h
#define ChaosFramework_AttributeEventHandlerQueueRunner_h


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
        void processBufferElement(AbstractAttributeEventHandlerPtr, ElementManagingPolicy&) throw(CException);
        
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
