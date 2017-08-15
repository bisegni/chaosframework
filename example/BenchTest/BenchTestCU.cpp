/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include <boost/thread.hpp>

#include "BenchTestCU.h"
#include "config.h"
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/bson/bson.h>
#include <chaos/common/bson/util/hex.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <chaos/cu_toolkit/control_manager/handler/DSInt32Handler.h>
#include <chaos/cu_toolkit/control_manager/handler/DSDoubleHandler.h>

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
void BenchTestCU::unitDefineActionAndDataset() throw(CException) {
    //set the base information
    const char *devIDInChar = reactorName.c_str();
    
    //set the default delay for the CU
    setDefaultScheduleDelay(CU_DELAY_FROM_TASKS);
    
    //add managed device di
    setDeviceID(devIDInChar);
    
    
    //setup the dataset
    addAttributeToDataSet("output_a",
                          "Reactor Output A",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("output_b",
                          "Reactor Output B",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    //setup the dataset
    addAttributeToDataSet("real_output_a",
                          "Reactor Output A",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("real_output_b",
                          "Reactor Output B",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("noise_a",
                          "Computed noise",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    addAttributeToDataSet("noise_b",
                          "Computed noise",
                          DataType::TYPE_DOUBLE,
                          DataType::Output);
    
    
    addInputDoubleAttributeToDataSet<BenchTestCU>("input_a",
                                                  "Reactor Output A",
                                                  this,
                                                  &BenchTestCU::setControlA);
    
    addInputDoubleAttributeToDataSet<BenchTestCU>("input_b",
                                                  "Reactor Output B",
                                                  this,
                                                  &BenchTestCU::setControlB);
}

void BenchTestCU::unitDefineDriver(std::vector<chaos::cu::driver_manager::driver::DrvRequestInfo>& neededDriver) {
	
}

//From base class
void BenchTestCU::unitInit() throw(CException) {
    reactorInstance->reset();
    cycleCount = 0;
    lastExecutionTime = steady_clock::now();
}

//From base class
void BenchTestCU::unitStart() throw(CException) {
    
}

//From base class
void BenchTestCU::unitRun() throw(CException) {
    cycleCount++;
    
    //get new data wrapper instance filled
    //with mandatory data
    chaos::common::data::CDataWrapper *acquiredData = getNewDataWrapper();
    if(!acquiredData) return;
    
    boost::shared_lock<boost::shared_mutex> lock(_setControlValueMutext);
    
    reactorInstance->compute_output();
    reactorInstance->update_output();
    
    acquiredData->addDoubleValue("output_a", reactorInstance->y_trasmessa[0]);
    acquiredData->addDoubleValue("output_b", reactorInstance->y_trasmessa[1]);
    acquiredData->addDoubleValue("real_output_a", reactorInstance->y[0]);
    acquiredData->addDoubleValue("real_output_b", reactorInstance->y[1]);
    acquiredData->addDoubleValue("noise_a", reactorInstance->d[0]);
    acquiredData->addDoubleValue("noise_b", reactorInstance->d[1]);

    //LAPP_ << " Output_A=" << reactorInstance->y[0] << " Output_B="<< reactorInstance->y[1];
    //check microseconds diff
    boost::chrono::microseconds diff = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now() - lastExecutionTime);
    if(diff.count() >= 1000000) {
        //print every second the value
        LAPP_ << "Cycle count =" << cycleCount << " micro sec passed=" << diff.count() << " Output_A=" << reactorInstance->y[0] << " Output_B="<< reactorInstance->y[1];
        cycleCount = 0;
        lastExecutionTime = boost::chrono::steady_clock::now();
    }
    pushDataSet(acquiredData);
    
    reactorInstance->compute_state();
    
}

//From base class
void BenchTestCU::unitStop() throw(CException) {
}

//From base class
void BenchTestCU::unitDeinit() throw(CException) {
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