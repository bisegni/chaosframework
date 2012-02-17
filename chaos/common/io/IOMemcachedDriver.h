/*	
 *	IOMemcachedDriver.h
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

#ifndef IOMemcachedDriver_H
#define IOMemcachedDriver_H

#include <string>
#include <libmemcached/memcached.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "IODataDriver.h"

namespace chaos{ 
    using namespace std;
    using namespace boost;
    
    class IOMemcachedDriver : public IODataDriver {
        boost::mutex useOutputChannelMutex;
        boost::mutex useInputChannelMutex;
        memcached_st *memClient;
        memcached_st *memClientRead;
    public:
        
        IOMemcachedDriver();
        virtual ~IOMemcachedDriver();
        
        /*
         * Init method, the has map has all received value for configuration
         * every implemented driver need to get all needed configuration param
         */
        void init() throw(CException);
        
        void deinit() throw(CException);
        
        /*
         * This method will cache all object passed to driver
         */
        void storeData(CDataWrapper*) throw(CException);
        
        /*
         * This method retrive the cached object by his key
         */
        virtual ArrayPointer<CDataWrapper>* retriveData(CDataWrapper*const)  throw(CException);
        /*
         * This method retrive the cached object by CSDawrapperUsed as query key and
         * return a pointer to the class ArrayPointer of CDataWrapper type
         */
        virtual ArrayPointer<CDataWrapper>* retriveData(string&)  throw(CException);
        /*
         * This method retrive the cached object by CSDawrapperUsed as query key and
         * return a pointer to the class ArrayPointer of CDataWrapper type
         */
        virtual char * retriveRawData(string& key, size_t *dim=NULL)  throw(CException);
        /*
         Update the driver configuration
         */
        CDataWrapper* updateConfiguration(CDataWrapper*);
    };
}
#endif
