//
//  DataManagment.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 2/26/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "DataManagment.h"

using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::cnd;
using namespace chaos::cnd::data;

#define DM_LAPP_ LAPP_ << "[DataManagment] - "

DataManagment::DataManagment() {
    
}

DataManagment::~DataManagment() {
    if(deviceDB) delete(deviceDB);
}

//! Initialize instance
void DataManagment::init(void *initData) throw(chaos::CException) {
    deviceDB = new CUSchemaDB(false);
    if(deviceDB) throw CException(0, "Error allocating device database", "DataManagment::init");
}

//! Start the implementation
void DataManagment::start() throw(chaos::CException) {
    
}


//! Start the implementation
void DataManagment::stop() throw(chaos::CException) {
    
}


//! Deinit the implementation
void DataManagment::deinit() throw(chaos::CException) {
    if(deviceDB) {
        delete(deviceDB);
        deviceDB = NULL;
    }
}


/*!
 
 */
CUSchemaDB *DataManagment::getDeviceDatabase(){
    return deviceDB;
}