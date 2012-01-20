//
//  OutputDataBuffer.cpp
//  ChaosFramework
//
//  Created by Claudio Bisegni on 17/03/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include "OutputDataBuffer.h"
#include "../../../common/global.h"

using namespace chaos;

OutputDataBuffer::OutputDataBuffer() {
}

OutputDataBuffer::OutputDataBuffer(IODataDriver *newIODriver):ioDriver(newIODriver) {
}

OutputDataBuffer::~OutputDataBuffer() {
}

/*
 Initialization method for output buffer
 */
void OutputDataBuffer::init(int threadNumber) throw(CException) {
    LAPP_ << "Initializing OutputDataBuffer";
    CObjectProcessingQueue<CDataWrapper>::init(threadNumber);
    
    if(ioDriver) {
        LAPP_ << "Initializing OutputDataBuffer IO Driver";
        (*ioDriver).init();
    } else throw CException(0, "No IO Driver provided", "OutputDataBuffer::init");
}

/*
 Deinitialization method for output buffer
 */
void OutputDataBuffer::deinit() throw(CException) {
    LAPP_ << "Deinitializing OutputDataBuffer";
    CObjectProcessingQueue<CDataWrapper>::deinit();
    
    if(ioDriver) {
        LAPP_ << "DeInitializing OutputDataBuffer IO Driver";
        (*ioDriver).deinit();
    } else throw CException(0, "No IO Driver provided", "OutputDataBuffer::init");
}

/*
 Process the oldest element in buffer
 */
void OutputDataBuffer::processBufferElement(CDataWrapper *rowData)  throw(CException){
        //check if livedata pointer is null
    if(!ioDriver || !rowData) return;
        
        //ithe the eventi lsitener has been provided, we need to check if the 
        //data si ammisible to be pushed
   
        //store data with live driver
    ioDriver->storeData(rowData);
}

/*
 Set the live io driver
 */
void OutputDataBuffer::setIODriver(IODataDriver *newIODriver) {
    ioDriver = newIODriver;
}

/*
 Return the internat instance of datadriver
 with shared_ptr
 */
IODataDriver *OutputDataBuffer::getIODataDriver() {
    return ioDriver;
}

/*
 Update the configuration
 */
CDataWrapper* OutputDataBuffer::updateConfiguration(CDataWrapper *newConfigration) {
    CHAOS_ASSERT(ioDriver);
    ioDriver->updateConfiguration(newConfigration);
    return NULL;
}