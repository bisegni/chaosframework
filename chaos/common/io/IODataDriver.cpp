//
//  IODataDriver.cpp
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 12/05/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include <chaos/common/global.h>
#include <chaos/common/io/IODataDriver.h>

#define IODataDriverLOG_HEAD "[IODataDriver] - "

#define IODataDriverLAPP	LAPP_ << IODataDriverLOG_HEAD
#define IODataDriverLDBG	LDBG_ << IODataDriverLOG_HEAD << __PRETTY_FUNCTION__ << " - "
#define IODataDriverLERR	LERR_ << IODataDriverLOG_HEAD << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "

using namespace chaos;
using namespace chaos::common::io;
using namespace chaos::common::utility;
using namespace chaos::common::data;


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::init(void *init_parameter){
    
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODataDriver::deinit() {
    
}

int IODataDriver::removeData(const std::string& key,
                             uint64_t start_ts,
                             uint64_t end_ts) {
    return 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ArrayPointer<CDataWrapper>*  IODataDriver::retriveData(const std::string& key, CDataWrapper*const) {
 //   boost::mutex::scoped_lock l(iomutex);

    //check for key length
    return retriveData(key);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
ArrayPointer<CDataWrapper>* IODataDriver::retriveData(const std::string& key)  {
  //  boost::mutex::scoped_lock l(iomutex);
    
    ArrayPointer<CDataWrapper> *result = new ArrayPointer<CDataWrapper>();
    
    char *value = retriveRawData(key);
    if (value) {
        //some value has been received
        //allocate the data wrapper object with serialization got from memcached
        //CDataWrapper *dataWrapper =
        result->add(new CDataWrapper(value));
        free(value);
    }
    return result;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
CDataWrapper* IODataDriver::updateConfiguration(CDataWrapper*){
    return NULL;
}
