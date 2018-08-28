//
//  LiveDataFetcher.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 1/5/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/chaos_constants.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/ui_toolkit/caching/LiveDataFetcher.h>

using namespace chaos;
using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::ui::chaching;

LiveDataFetcher::LiveDataFetcher(IODataDriver *_dataDriver):dataDriver(_dataDriver) {
    
}

LiveDataFetcher::~LiveDataFetcher() {
    if(dataDriver){
        dataDriver->deinit();
        delete(dataDriver);
    }
}

/*
 Update the driver configuration
 */
CDataWrapper* LiveDataFetcher::updateConfiguration(CDataWrapper *configuration) {
    if(dataDriver) dataDriver->updateConfiguration(configuration);
    return NULL;
}

// Initialize instance
void LiveDataFetcher::init(void *configuration) throw(chaos::CException) {
    if(dataDriver) {
        dataDriver->init(NULL);
    }
    updateConfiguration(static_cast<CDataWrapper*>(configuration));
}

// Start the implementation
void LiveDataFetcher::start() throw(chaos::CException) {
    
}

// Deinit the implementation
void LiveDataFetcher::deinit() throw(chaos::CException) {
    if(dataDriver) {
        dataDriver->deinit();
    }
}

void LiveDataFetcher::getData(const std::string& key, CDataWrapper& newData, uint64_t& ts) {
    currentRawDataPtr.reset(dataDriver->retriveRawData(key));
    
        //create key/value chaos object
    newData.setSerializedData(currentRawDataPtr.get());
    
        //extras the timestamp of the pack
    ts = newData.getInt64Value(DataPackCommonKey::DPCK_TIMESTAMP);
}