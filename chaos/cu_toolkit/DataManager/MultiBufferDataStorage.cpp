//
//  MultiBufferDataStorage.cpp
//  ChaosFramework
//
//  Created by Claudio Bisegni on 20/03/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include <boost/lexical_cast.hpp>
#include "MultiBufferDataStorage.h"
#include "../../common/cconstants.h"
#include "../../common/global.h"

using namespace chaos;
using namespace std;

/*
 Constructor
 */
MultiBufferDataStorage::MultiBufferDataStorage() {
    liveMsecOffset   = 0;
    liveLastMsecSent = 0;
    
        //timing util allocation
    timingUtil = new TimingUtil();
    
    liveOBuffer = NULL;
    hstOBuffer = NULL;
}

/*
 Destructor
 */
MultiBufferDataStorage::~MultiBufferDataStorage() {
        //timing util deallocation
    if(timingUtil) delete(timingUtil);
}

/*
 Initialize the Multisequence buffer
 */
void MultiBufferDataStorage::init(CDataWrapper *configuration) {
    int32_t liveThreadNumber = 0;
    CHAOS_ASSERT(configuration)
    LAPP_ << "Initialization Multi Sequencial Buffer";
    auto_ptr<CDataWrapper>  dmStartupConfiguration(configuration->getCSDataValue(LiveHistoryMDSConfiguration::CS_DM_CONFIGURATION));
    
    if(liveOBuffer) liveOBuffer->setEndElaborationListener(this);
    if(hstOBuffer) hstOBuffer->setEndElaborationListener(this);
    
    if(dmStartupConfiguration->hasKey(LiveHistoryMDSConfiguration::CS_DM_OUTPUT_BUFFER_THREAD_NUM)){
        liveThreadNumber = dmStartupConfiguration->getInt32Value(LiveHistoryMDSConfiguration::CS_DM_OUTPUT_BUFFER_THREAD_NUM);
    } else {
        LAPP_CFG_ << "No configuration for thread number found, use the default value";
        liveThreadNumber = DEFAULT_OBUFFER_THREAD_NUMEBR;
    }
    
    LAPP_CFG_ << "Configure Output buffer with " << liveThreadNumber << " thread each one";
    if(liveOBuffer) liveOBuffer->init(liveThreadNumber);
    if(hstOBuffer) hstOBuffer->init(liveThreadNumber);

}

/*
 Deinitialize the Multisequence buffer
 */
void MultiBufferDataStorage::deinit() {
        //deinit the live buffer
    if(liveOBuffer) {
        (*liveOBuffer).deinit();    
        (*liveOBuffer).joinBufferThread();
        delete(liveOBuffer);
        liveOBuffer = NULL;
    }
    
    if(hstOBuffer) {
        (*hstOBuffer).deinit();    
        (*hstOBuffer).joinBufferThread();
        delete(hstOBuffer);
        hstOBuffer = NULL;
    }
}

/*
 Set the first buffer
 */
void MultiBufferDataStorage::setLiveOBuffer(OutputDataBuffer *newLiveOBuffer) {
        //check  that there is no other driver(only one can be set
    CHAOS_ASSERT(!liveOBuffer)
    liveOBuffer = newLiveOBuffer;
    if(liveOBuffer) liveOBuffer->tag = TAG_BUFFER_LIVE;
}

    /*
     Set the first buffer
     */
void MultiBufferDataStorage::setHstOBuffer(OutputDataBuffer *newHstOBuffer) {
        //check  that there is no other driver(only one can be set
    CHAOS_ASSERT(!hstOBuffer)
    hstOBuffer = newHstOBuffer;    
    if(hstOBuffer) hstOBuffer->tag = TAG_BUFFER_HST;
}

/*
 
 */
void MultiBufferDataStorage::pushDataSet(CDataWrapper *rowData) {
    CHAOS_ASSERT(liveOBuffer)
    liveOBuffer->push(rowData);
}

/*
 
 */
bool MultiBufferDataStorage::elementWillBeDiscarded(int bufferTag, CDataWrapper *rowData) {
    return true;
}

/*
 
 */
bool MultiBufferDataStorage::elementWillBeProcessed(int bufferTag, CDataWrapper *rowData)  {
    return true;
}

/*
 Permit to be live configurable, all parameter are here configured
 */
CDataWrapper* MultiBufferDataStorage::updateConfiguration(CDataWrapper *newConfiguration) {
    LAPP_CFG_ << "Configuration for Multi Sequencial Buffer";
        //chec if is present the DataManager configuration
    
    if(newConfiguration->hasKey(LiveHistoryMDSConfiguration::CS_DM_OUTPUT_BUFFER_LIVE_TIME))
        liveMsecOffset = newConfiguration->getInt32Value(LiveHistoryMDSConfiguration::CS_DM_OUTPUT_BUFFER_LIVE_TIME);

    if(newConfiguration->hasKey(LiveHistoryMDSConfiguration::CS_DM_OUTPUT_BUFFER_HST_TIME))
        historyUSecOffset = newConfiguration->getInt32Value(LiveHistoryMDSConfiguration::CS_DM_OUTPUT_BUFFER_HST_TIME);
    
        //update the driver configration
    if(liveOBuffer != NULL) liveOBuffer->updateConfiguration(newConfiguration);
    if(hstOBuffer != NULL) hstOBuffer->updateConfiguration(newConfiguration);
    return NULL;
}

/*
 Return the IODataDriver for the input buffer tag 
 */
IODataDriver* MultiBufferDataStorage::getBufferIODataDriver(int bufferTag) {
    switch (bufferTag) {
        case TAG_BUFFER_LIVE:
            return liveOBuffer->getIODataDriver();
        break;
            
        case TAG_BUFFER_HST:
            return hstOBuffer->getIODataDriver();
        break;
    }
    return NULL;
}
