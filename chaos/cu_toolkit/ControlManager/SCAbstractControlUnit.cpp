/*
 *	SCAbstractControlUnit.cpp
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
#include <chaos/cu_toolkit/ControlManager/SCAbstractControlUnit.h>
using namespace chaos;
using namespace chaos::cu;
using namespace cu::control_manager::slow_command;

#define LCCU_ LAPP_ << "[Slow Command Control Unit:"<<getCUInstance()<<"] - "

SCAbstractControlUnit::SCAbstractControlUnit() {
    slowCommandExecutor = new cu::control_manager::slow_command::SlowCommandExecutor(cuInstance, this);

}

SCAbstractControlUnit::~SCAbstractControlUnit() {
    if(slowCommandExecutor) {
        delete(slowCommandExecutor);
    }
}

/*!
 Initialize the Custom Contro Unit and return the configuration
 */
void SCAbstractControlUnit::init() throw(CException) {
    LCCU_ << "Initializing slow command sandbox" << DeviceSchemaDB::getDeviceID();
    utility::StartableService::initImplementation(slowCommandExecutor, (void*)NULL, "Slow Command Executor", "SCAbstractControlUnit::init");
    //associate the data storage
    slowCommandExecutor->commandSandbox.keyDataStorage = AbstractControlUnit::keyDataStorage;
}

/*!
 Deinit the Control Unit
 */
void SCAbstractControlUnit::deinit() throw(CException) {
    LCCU_ << "Deinitialize sandbox deinitialization for device:" << DeviceSchemaDB::getDeviceID();
    utility::StartableService::deinitImplementation(slowCommandExecutor, "Slow Command Executor", "SCAbstractControlUnit::deinit");
    //deassociate the data storage
    slowCommandExecutor->commandSandbox.keyDataStorage = NULL;
}

/*!
 Starto the  Control Unit scheduling for device
 */
void SCAbstractControlUnit::start() throw(CException) {
    LCCU_ << "Start sandbox for device:" << DeviceSchemaDB::getDeviceID();
    utility::StartableService::startImplementation(slowCommandExecutor, "Slow Command Executor", "SCAbstractControlUnit::start");

}

/*!
 Stop the Custom Control Unit scheduling for device
 */
void SCAbstractControlUnit::stop() throw(CException) {
    LCCU_ << "Stop slow command executor for device:" << DeviceSchemaDB::getDeviceID();
    utility::StartableService::stopImplementation(slowCommandExecutor, "Slow Command Executor", "SCAbstractControlUnit::stop");

}

//! Perform a command registration
/*!
 An instance of the command si registered within the executor.
 */
void SCAbstractControlUnit::setDefaultCommand(const char * dafaultCommandName) {
    CHAOS_ASSERT(slowCommandExecutor);
    slowCommandExecutor->setDefaultCommand(dafaultCommandName);
}