    //
    //  IODataDriver.h
    //  ControlSystemLib
    //
    //  Created by Claudio Bisegni on 13/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
#ifndef IODataDriver_H
#define IODataDriver_H
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <chaos/common/exception/CException.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/utility/ArrayPointer.h>
namespace chaos{ 
    using namespace std;
    
    /*
     * History Output driver base abstract classe, that define two method to control
     * the initialization and history persistence of the data
     */
    
    class IODataDriver: public Configurable {
    public:
        /*
         * Init method, the has map has all received value for configuration
         * every implemented driver need to get all needed configuration param
         */
        virtual void init() throw(CException) = 0;
        /*
         * DeInit method
         */
        virtual void deinit() throw(CException) = 0;
        
        /*
         * This method cache all object passed to driver
         */
        virtual void storeData(CDataWrapper*) throw(CException) = 0;
        
        
        /*
         * This method retrive the cached object by CSDawrapperUsed as query key and
         * return a pointer to the class ArrayPointer of CDataWrapper type
         */
        virtual ArrayPointer<CDataWrapper>* retriveData(CDataWrapper*const)  throw(CException) = 0;
        
        /*
         * This method retrive the cached object by CSDawrapperUsed as query key and
         * return a pointer to the class ArrayPointer of CDataWrapper type
         */
        virtual ArrayPointer<CDataWrapper>* retriveData(string&)  throw(CException) = 0;
  
        /*
         * This method retrive the cached object by CSDawrapperUsed as query key and
         * return a pointer to the class ArrayPointer of CDataWrapper type
         */
        virtual char * retriveRawData(string&, size_t*)  throw(CException) = 0;
        
        /*
         Update the driver configuration
         */
        virtual CDataWrapper* updateConfiguration(CDataWrapper*) = 0;
    };
}
#endif

