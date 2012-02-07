//
//  IOMemcachedDriver.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 13/03/11.
//  Copyright 2011 INFN. All rights reserved.
//

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
