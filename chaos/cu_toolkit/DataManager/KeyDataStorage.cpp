//
//  KeyDataStorage.cpp
//  ControlSystemLib
//
//  Created by Claudio Bisegni on 12/03/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include "KeyDataStorage.h"
#include "../../common/global.h"
#include "../../common/cconstants.h"

using namespace chaos;
using namespace std;

KeyDataStorage::KeyDataStorage(const char * key):dataSetKey(key),keyData(new CDataWrapper()) {
        //associate the key to datawrapper used for retrive last personal data
    keyData->addStringValue(ControlSystemValueConstant::CS_CSV_DEVICE_ID, dataSetKey);
}

KeyDataStorage::KeyDataStorage(std::string& key):dataSetKey(key) {
    
}

KeyDataStorage::~KeyDataStorage() {
    
}

void KeyDataStorage::init(CDataWrapper *startConfig) {
        //send message to subclass
    MultiBufferDataStorage::init(startConfig);
        //i delete this class as listener because i send  the data into
        //in the same time int the two buffer
    if(liveOBuffer) liveOBuffer->setEndElaborationListener(NULL);
    if(hstOBuffer) hstOBuffer->setEndElaborationListener(NULL);
        //get reference to live iodriver from live buffer
    liveIODriver = liveOBuffer->getIODataDriver();
}

/*
 Return a new instace for the CSDatawrapped filled
 with default madatory data
 */
CDataWrapper* KeyDataStorage::getNewDataWrapper() {
    CDataWrapper *result = new CDataWrapper();
        //add key to datawrapper
    result->addStringValue(ControlSystemValueConstant::CS_CSV_DEVICE_ID, dataSetKey);
        //add timestamp to datawrapper
    result->addInt64Value(ControlSystemValueConstant::CS_CSV_TIME_STAMP, timingUtil->getTimeStamp());
    return result;
}

/*
 Retrive the data from Live Storage
 */
ArrayPointer<CDataWrapper>* KeyDataStorage::getLastDataSet() {
        //retrive data from cache for the key managed by
        //this instance of keydatastorage
    CHAOS_ASSERT(liveIODriver);
    return liveIODriver->retriveData(dataSetKey);
}

/*
 Retrive the data from History Storage
 */
ArrayPointer<CDataWrapper>* KeyDataStorage::getHistoricalDataSet(CDataWrapper *searchIption) {
    return NULL;
}

/*
 
 */
CDataWrapper* KeyDataStorage::updateConfiguration(CDataWrapper *newConfiguration) {
    MultiBufferDataStorage::updateConfiguration(newConfiguration);
    
    return NULL;
}