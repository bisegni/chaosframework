//
//  AttributeEventHandlerQueueRunner.cpp
//  ChaosFramework
//
//  Created by Bisegni Claudio on 05/11/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

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
void AttributeEventHandlerQueueRunner::processBufferElement(AbstractAttributeEventHandlerPtr eventHanlder) throw(CException) {
    eventHanlder->call();
} 