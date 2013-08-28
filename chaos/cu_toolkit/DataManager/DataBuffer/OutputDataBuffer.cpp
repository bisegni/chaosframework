/*	
 *	OutputDataBuffer.cpp
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
#include "OutputDataBuffer.h"
#include "../../../common/global.h"

using namespace chaos;
using namespace chaos::common::data;

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
void OutputDataBuffer::processBufferElement(CDataWrapper *rowData, ElementManagingPolicy&)  throw(CException){
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
    if(ioDriver) ioDriver->updateConfiguration(newConfigration);
    return NULL;
}
