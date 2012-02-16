//
//  DatasetAttributeEventDispatcher.cpp
//  ChaosFramework
//
//  Created by Bisegni Claudio on 06/11/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include <vector>
#include "DatasetAttributeEventDispatcher.h"

using namespace chaos;

DatasetAttributeEventDispatcher::DatasetAttributeEventDispatcher() {
   
}

DatasetAttributeEventDispatcher::~DatasetAttributeEventDispatcher() {
}

/*
 Initialization of dispatcher
 */
void DatasetAttributeEventDispatcher::init() {
        //startup the default queue
     defaultQueue.reset(new AttributeEventHandlerQueueRunner());
    defaultQueue->init(1);
}

/*
 Deinitialization of the dispatcher
 */
void DatasetAttributeEventDispatcher::deinit() {
    //init the defautl queue
    defaultQueue->deinit();
    
    //get all distinct instance of queue
    AttributeEventHandlerQueueRunner *ptr;
    multiset< AttributeEventHandlerQueueRunner* > queueInstance;
    multiset< AttributeEventHandlerQueueRunner* >::iterator queueInstanceIterator;
    map<string, shared_ptr<AttributeEventHandlerQueueRunner> >::iterator iter = attributeQueueMap.begin();
    while (iter != attributeQueueMap.end()) {
        ptr = ((*iter).second).get();
        if(queueInstance.find(ptr) != queueInstance.end()){
            queueInstance.insert(ptr);
        }
        iter++;
    }
    
        //now all instacend are deinit
    queueInstanceIterator = queueInstance.begin();
    while (queueInstanceIterator != queueInstance.end()) {
        (*queueInstanceIterator)->deinit();
        queueInstanceIterator++;
    }
}

/*
 Add the attribute to the default queue
 */
void DatasetAttributeEventDispatcher::addAttributeToDefaultQueue(string& attributeName)  throw(CException) {
    if(!attributeQueueMap.count(attributeName)) {
        throw CException(0, "Attribute name already associated to a queue", "DatasetAttributeEventDispatcher::addAttributeToDefaultQueue");
    }
        //add queue for handler
    attributeQueueMap.insert(make_pair<string, shared_ptr<AttributeEventHandlerQueueRunner> >(attributeName, defaultQueue));
}

/*
 Add the attribute to the default queue
 */
void DatasetAttributeEventDispatcher::addAttributeToNewQueue(string& attributeName) throw(CException) {
    if(!attributeQueueMap.count(attributeName)) {
        throw CException(0, "Attribute name already associated to a queue", "DatasetAttributeEventDispatcher::addAttributeToNewQueue");
    }
  
    shared_ptr<AttributeEventHandlerQueueRunner> newQueue(new AttributeEventHandlerQueueRunner());
    newQueue->init(1);
    
    attributeQueueMap.insert(make_pair<string, shared_ptr<AttributeEventHandlerQueueRunner> >(attributeName, newQueue));
}


/*
 Add the attribute handler inthe same queue of other attribute
 */
void DatasetAttributeEventDispatcher::addAttributeHandler(AbstractAttributeEventHandlerPtr attributeHandler) {
    if(!attributeHandler) return;
        //add the new handler
    attributeHandlerMap.insert(make_pair<string, AbstractAttributeEventHandlerPtr>(attributeHandler->getAttributeName(), attributeHandler));
}
