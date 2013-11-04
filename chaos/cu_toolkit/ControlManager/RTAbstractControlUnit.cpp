/*
 *	RTAbstractControlUnit.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/event/channel/InstrumentEventChannel.h>

#include <chaos/cu_toolkit/ControlManager/RTAbstractControlUnit.h>

using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::cu;

#define RTCULAPP_ LAPP_ << "[Real Time Control Unit:"<<getCUInstance()<<"] - "

RTAbstractControlUnit::RTAbstractControlUnit():AbstractControlUnit(CUType::RTCU) {
    //allocate the thread for the scheduler
    schedulerThread = new CThread(this);
    schedulerThread->setDelayBeetwenTask(1000000);

    //allocate the handler engine
    attributeHandlerEngine = new DSAttributeHandlerExecutionEngine(this);
}

RTAbstractControlUnit::~RTAbstractControlUnit() {
    //release handler engine
    if(attributeHandlerEngine) {
        delete attributeHandlerEngine;
    }

    //release scheduler thread
    if(schedulerThread) {
        delete(schedulerThread);
        schedulerThread = NULL;
    }
}

void RTAbstractControlUnit::setDefaultScheduleDelay(uint32_t _defaultScheduleDelay) {
    defaultScheduleDelay = _defaultScheduleDelay;
}

/*
 fill the CDataWrapper with AbstractCU system configuration, this method
 is called after getStartConfiguration directly by sandbox. in this method
 are defined the action for the input element of the dataset
 */
void RTAbstractControlUnit::_defineActionAndDataset(CDataWrapper& setupConfiguration)  throw(CException) {
    AbstractControlUnit::_defineActionAndDataset(setupConfiguration);
    //add the scekdule dalay for the sandbox
    if(defaultScheduleDelay){
        //in this case ovverrride the config file
    	setupConfiguration.addInt32Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY , defaultScheduleDelay);
    }
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void RTAbstractControlUnit::init(void *initData) throw(CException) {
	//call parent impl
	AbstractControlUnit::init(initData);
	
    RTCULAPP_ << "Initialize the DSAttribute handler engine for device:" << DatasetDB::getDeviceID();
    utility::StartableService::initImplementation(attributeHandlerEngine, (void*)NULL, "DSAttribute handler engine", "RTAbstractControlUnit::init");
}

/*!
 Starto the  Control Unit scheduling for device
 */
void RTAbstractControlUnit::start() throw(CException) {
	//call parent impl
	AbstractControlUnit::start();
	
    RTCULAPP_ << "Starting thread for device:" << DatasetDB::getDeviceID();
    threadStartStopManagment(true);
    RTCULAPP_ << "Thread started for device:" << DatasetDB::getDeviceID();
}

/*!
 Stop the Custom Control Unit scheduling for device
 */
void RTAbstractControlUnit::stop() throw(CException) {
	//call parent impl
	AbstractControlUnit::stop();
	
    //manage the thread
    RTCULAPP_ << "Stopping thread for device:" << DatasetDB::getDeviceID();
    threadStartStopManagment(false);
    RTCULAPP_ << "Thread for device stopped:" << DatasetDB::getDeviceID();
}

/*!
 Init the  RT Control Unit scheduling for device
 */
void RTAbstractControlUnit::deinit() throw(CException) {
	//call parent impl
	AbstractControlUnit::deinit();
	
    RTCULAPP_ << "Deinitializing the DSAttribute handler engine for device:" << DatasetDB::getDeviceID();
    utility::StartableService::deinitImplementation(attributeHandlerEngine, "DSAttribute handler engine", "RTAbstractControlUnit::deinit");
}

/*
 Add a new handler
 */
void RTAbstractControlUnit::addHandlerForDSAttribute(cu::handler::DSAttributeHandler * classHandler)  throw (CException) {
    if(!classHandler) return;
    //add the handler
    attributeHandlerEngine->addHandlerForDSAttribute(classHandler);
}

/*!
 return the appropriate thread for the device
 */
void RTAbstractControlUnit::threadStartStopManagment(bool startAction) throw(CException) {
    if(startAction) {
        if(schedulerThread && !schedulerThread->isStopped()){
            RTCULAPP_ << "thread already running";
            throw CException(-5, "Thread for device already running", "RTAbstractControlUnit::threadStartStopManagment");
        }
        
        if(schedulerThread) {
            schedulerThread->start();
            return;
        }
        
        //set the defautl scehduling rate to 1 sec
        schedulerThread->start();
        schedulerThread->setThreadPriorityLevel(sched_get_priority_max(SCHED_RR), SCHED_RR);
    } else {
        if(schedulerThread->isStopped()){
            RTCULAPP_ << "thread already runnign";
            throw CException(-5, "Thread for device already running", "RTAbstractControlUnit::threadStartStopManagment");
        }
        RTCULAPP_ << "Stopping and joining scheduling thread";
        schedulerThread->stop(true);
        RTCULAPP_ << "Thread stopped";
    }
    
}

/*
 Return a new instance of CDataWrapper filled with a mandatory data
 according to key
 */
CDataWrapper *RTAbstractControlUnit::getNewDataWrapper() {
    return keyDataStorage->getNewDataWrapper();
}


/*
 Send device data to output buffer
 */
void RTAbstractControlUnit::pushDataSet(CDataWrapper *acquiredData) {
    //send data to related buffer
    keyDataStorage->pushDataSet(acquiredData);
}

/*!
 Event for update some CU configuration
 */
CDataWrapper* RTAbstractControlUnit::updateConfiguration(CDataWrapper* updatePack, bool& detachParam) throw (CException) {
    CDataWrapper *result = AbstractControlUnit::updateConfiguration(updatePack, detachParam);
    if(updatePack->hasKey(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY)){
        //we need to configure the delay  from a run() call and the next
        uint64_t uSecdelay = updatePack->getUInt64Value(CUDefinitionKey::CS_CM_THREAD_SCHEDULE_DELAY);
        //check if we need to update the scehdule time
        if(uSecdelay != schedulerThread->getDelayBeetwenTask()){
            RTCULAPP_ << "Update schedule delay in:" << uSecdelay << " microsecond";
            schedulerThread->setDelayBeetwenTask(uSecdelay);
            //send envet for the update
            //----------------------
            // we need to optimize and be sure that event channel
            // is mandatory so we can left over the 'if' check
            //----------------------
            if(deviceEventChannel) deviceEventChannel->notifyForScheduleUpdateWithNewValue(DatasetDB::getDeviceID(), uSecdelay);
        }
    }
    return result;
}

/*
 Execute the scehduling for the device
 */
void RTAbstractControlUnit::executeOnThread() throw(CException) {
    unitRun();
    //check if we are in sequential or in threaded mode
}

/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* RTAbstractControlUnit::setDatasetAttribute(CDataWrapper *datasetAttributeValues, bool& detachParam) throw (CException) {
    attributeHandlerEngine->executeHandler(datasetAttributeValues);
    
    //at this time notify the wel gone setting of comand
    if(deviceEventChannel) deviceEventChannel->notifyForAttributeSetting(DatasetDB::getDeviceID(), 0);
    return NULL;
}
