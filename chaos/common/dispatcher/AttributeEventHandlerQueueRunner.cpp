/*	
 *	AttributeEventHandlerQueueRunner.cpp
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
#include "AttributeEventHandlerQueueRunner.h"
using namespace chaos;

/*
 Initialization method for output buffer
 */
void AttributeEventHandlerQueueRunner::init(int threadNumber) throw(CException) {
    CObjectProcessingQueue<AbstractAttributeEventHandler>::init(threadNumber);

}

/*
 Deinitialization method for output buffer
 */
void AttributeEventHandlerQueueRunner::deinit() throw(CException) {
        //mutex::scoped_lock lockAction(actionAccessMutext);
    CObjectProcessingQueue<AbstractAttributeEventHandler>::clear();
    CObjectProcessingQueue<AbstractAttributeEventHandler>::deinit();
}

/*
 override the push method for ObjectProcessingQueue<AbstractAttributeEventHandler> superclass
 */
bool AttributeEventHandlerQueueRunner::push(AbstractAttributeEventHandlerPtr actionParam) throw(CException) {
    return CObjectProcessingQueue<AbstractAttributeEventHandler>::push(actionParam);
}



/*
 process the element action to be executed
 */
void AttributeEventHandlerQueueRunner::processBufferElement(AbstractAttributeEventHandlerPtr eventHanlder, ElementManagingPolicy& elementPolicy) throw(CException) {
    eventHanlder->call();
}
