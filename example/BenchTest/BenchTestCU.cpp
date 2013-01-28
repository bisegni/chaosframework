/*
 *	BenchTestCU.cpp
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
#include <boost/thread.hpp>

#include "BenchTestCU.h"
#include "config.h"
#include <chaos/common/global.h>
#include <chaos/common/cconstants.h>
#include <chaos/common/bson/bson.h>
#include <chaos/common/bson/util/hex.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/cu_toolkit/ControlManager/handler/DSInt32Handler.h>
#include <chaos/cu_toolkit/ControlManager/handler/DSDoubleHandler.h>

#include <cmath>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
using namespace chaos;


/*
 Construct a new CU with an identifier
 */
BenchTestCU::BenchTestCU(std::string& rName, Batch_Reactor *rInstance) {
    reactorName = rName;
    reactorInstance = rInstance;
}

/*
 Destructor a new CU with an identifier
 */
BenchTestCU::~BenchTestCU() {
    if(reactorInstance) delete(reactorInstance);
}

/*
 Return the default configuration
 */
void BenchTestCU::defineActionAndDataset(CDataWrapper& cuSetup) throw(CException) {
    //set the base information
    const char *devIDInChar = reactorName.c_str();
    
    //set the default delay for the CU
    setDefaultScheduleDelay(CU_DELAY_FROM_TASKS);
    
    //add managed device di
    addDeviceId(devIDInChar);
    
    
    //setup the dataset
    addAttributeToDataSet(devIDInChar,
                          "output_a",
                          "Reactor Output A",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet(devIDInChar,
                          "output_b",
                          "Reactor Output B",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    
    addInputDoubleAttributeToDataSet<BenchTestCU>(devIDInChar,
                                                  "input_a",
                                                  "Reactor Output A",
                                                  this,
                                                  &BenchTestCU::setControlA);
    
    addInputDoubleAttributeToDataSet<BenchTestCU>(devIDInChar,
                                                  "input_b",
                                                  "Reactor Output B",
                                                  this,
                                                  &BenchTestCU::setControlB);
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void BenchTestCU::init(CDataWrapper *newConfiguration) throw(CException) {
    reactorInstance->reset();
    cycleCount = 0;
    lastExecutionTime = steady_clock::now();
}

/*
 Execute the Control Unit work
 */
void BenchTestCU::run(const string& deviceID) throw(CException) {
    const char *devIDInChar = reactorName.c_str();
    cycleCount++;
    
    //get new data wrapper instance filled
    //with mandatory data
    CDataWrapper *acquiredData = getNewDataWrapperForKey(devIDInChar);
    if(!acquiredData) return;
    
    boost::shared_lock<boost::shared_mutex> lock(_setControlValueMutext);
    
    reactorInstance->compute_output();
    reactorInstance->update_output();
    
    acquiredData->addDoubleValue("output_a", reactorInstance->y_trasmessa[0]);
    acquiredData->addDoubleValue("output_b", reactorInstance->y_trasmessa[1]);
    //LAPP_ << " Output_A=" << reactorInstance->y[0] << " Output_B="<< reactorInstance->y[1];
    //check microseconds diff
    boost::chrono::microseconds diff = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now() - lastExecutionTime);
    if(diff.count() >= 1000000) {
        //print every second the value
        LAPP_ << "Cycle count =" << cycleCount << " micro sec passed=" << diff.count() << " Output_A=" << reactorInstance->y[0] << " Output_B="<< reactorInstance->y[1];
        cycleCount = 0;
        lastExecutionTime = boost::chrono::steady_clock::now();
    }
    pushDataSetForKey(devIDInChar, acquiredData);
    
    reactorInstance->compute_state();
    
}

/*
 Execute the Control Unit work
 */
void BenchTestCU::stop(const string& deviceID) throw(CException) {
}

/*
 Deinit the Control Unit
 */
void BenchTestCU::deinit(const string& deviceID) throw(CException) {
}

/*
 */
void BenchTestCU::setControlA(const std::string& deviceID, const double& dValue) {
    boost::unique_lock< boost::shared_mutex > lock(_setControlValueMutext);
    reactorInstance->u[0] = dValue;
}

/*
 */
void BenchTestCU::setControlB(const std::string& deviceID, const double& dValue) {
    boost::unique_lock< boost::shared_mutex > lock(_setControlValueMutext);
    reactorInstance->u[1] = dValue;
}