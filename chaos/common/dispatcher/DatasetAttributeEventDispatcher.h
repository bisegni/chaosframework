/*	
 *	DatasetAttributeEventDispatcher.h
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
#ifndef ChaosFramework_DatasetAttributeEventDispatcher_h
#define ChaosFramework_DatasetAttributeEventDispatcher_h

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include "AttributeEventHandlerQueueRunner.h"

namespace chaos {
    
    using namespace std;
    using namespace boost;
    
    /*!
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
        
        /*!
         Init
         */
        void init();
        
        /*!
         Deinit
         */
        void deinit();
        
        /*!
         Add the attribute to the default queue
         */
        void addAttributeToDefaultQueue(string&) throw(CException);
        
        /*!
         Add the attribute to the default queue
         */
        void addAttributeToNewQueue(string&) throw(CException);
        
        /*!
         Add the attribute handler in the default queue
         */
        void addAttributeHandler(AbstractAttributeEventHandlerPtr);
        
        /*!
         Add the attribute handler in the default queue
         */
        void addAttributeHandlerInNewQueue(AbstractAttributeEventHandlerPtr);
        
    };
}
#endif
