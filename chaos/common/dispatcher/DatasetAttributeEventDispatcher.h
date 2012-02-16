//
//  DatasetAttributeEventDispatcher.h
//  ChaosFramework
//
//  Created by Bisegni Claudio on 05/11/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_DatasetAttributeEventDispatcher_h
#define ChaosFramework_DatasetAttributeEventDispatcher_h

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include "AttributeEventHandlerQueueRunner.h"

namespace chaos {
    
    using namespace std;
    using namespace boost;
    
    /*
     This class represent the abstract dispatcher for the pair
     attribute_name/method pointer. Thsi permit to serialize the call for some
     attribute
     */
    class DatasetAttributeEventDispatcher {
        shared_ptr<AttributeEventHandlerQueueRunner>  defaultQueue;
        
        map<string, shared_ptr<AttributeEventHandlerQueueRunner> > attributeQueueMap;
        multimap<string, AbstractAttributeEventHandlerPtr > attributeHandlerMap;
        
    public:
        DatasetAttributeEventDispatcher();
        
        ~DatasetAttributeEventDispatcher();
        
        /*
         Init
         */
        void init();
        
        /*
         Deinit
         */
        void deinit();
        
        /*
         Add the attribute to the default queue
         */
        void addAttributeToDefaultQueue(string&) throw(CException);
        
        /*
         Add the attribute to the default queue
         */
        void addAttributeToNewQueue(string&) throw(CException);
        
        /*
         Add the attribute handler in the default queue
         */
        void addAttributeHandler(AbstractAttributeEventHandlerPtr);
        
        /*
         Add the attribute handler in the default queue
         */
        void addAttributeHandlerInNewQueue(AbstractAttributeEventHandlerPtr);
        
    };
}
#endif
