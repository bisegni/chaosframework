/*	
 *	WorkerCU.cpp
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

#include "WorkerCU.h"
#include <chaos/common/global.h>
#include <chaos/common/cconstants.h>
#include <chaos/common/bson/bson.h>
#include <chaos/common/bson/util/hex.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h> 
#include <stdlib.h>
using namespace chaos;

#define SIMULATED_DEVICE_ID     "SIN_DEVICE"
#define DS_ELEMENT_1            "sinOutput"
#define DS_ELEMENT_2            "sinAltitude"
#define DS_ELEMENT_3            "sinPahseTime"

#define TEST_BUFFER_DIM         100
#define CU_DELAY_FROM_TASKS     1000000 //1Sec
#define ACTION_TWO_PARAM_NAME   "actionTestTwo_paramName"



WorkerCU::WorkerCU():AbstractControlUnit(),rng((const uint_fast32_t) time(0) ),one_to_hundred( -100, 100 ),randInt(rng, one_to_hundred) {
    //first we make some write
    _deviceID.assign(SIMULATED_DEVICE_ID);
    cuName = "WORKER_CU";
    numberOfResponse = 0;
    sinCompConst = (double)6.28/(double)360;
}

/*
 Construct a new CU with an identifier
 */
WorkerCU::WorkerCU(string &customDeviceID):rng((const uint_fast32_t) time(0) ),one_to_hundred( -100, 100 ),randInt(rng, one_to_hundred){
    _deviceID = customDeviceID;
    cuName = "WORKER_CU";
    numberOfResponse = 0;
}

/*
 Destructor a new CU with an identifier
 */
WorkerCU::~WorkerCU() {
    
}

/*
 Return the default configuration
 */
void WorkerCU::defineActionAndDataset(CDataWrapper& cuSetup) throw(CException) {
    //set the base information
    const char *devIDInChar = _deviceID.c_str();
    cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_NAME, "SIN_CU");
    cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION, "This is a beautifull CU");
    
    
    //set the default delay for the CU
    setDefaultScheduleDelay(CU_DELAY_FROM_TASKS);
    
    //add managed device di
    addDeviceId(_deviceID);
    
    //add custom action
    AbstActionDescShrPtr  
    actionDescription = addActionDescritionInstance<WorkerCU>(this, 
                                                              &WorkerCU::actionTestOne, 
                                                              "actionTestOne", 
                                                              "comandTestOne this action will do some beautifull things!");
    
    actionDescription = addActionDescritionInstance<WorkerCU>(this, 
                                                              &WorkerCU::resetStatistic, 
                                                              "resetStatistic", 
                                                              "resetStatistic this action will reset  all cu statistic!");
    
    actionDescription = addActionDescritionInstance<WorkerCU>(this, 
                                                              &WorkerCU::actionTestTwo, 
                                                              "actionTestTwo", 
                                                              "comandTestTwo, this action will do some beautifull things!");
    
    //add param to second action
    actionDescription->addParam(ACTION_TWO_PARAM_NAME, 
                                DataType::TYPE_INT32, 
                                "integer 32bit action param description for testing purpose");
    
    //setup the dataset
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_1,
                          "The sin value in output",
                          DataType::TYPE_DOUBLE, 
                          DataType::Output);
    //setup the dataset
    addAttributeToDataSet(devIDInChar,
                          "sinWave",
                          "The sin waveform",
                          DataType::TYPE_BYTEARRAY, 
                          DataType::Output);
    
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_2,
                          "The input altitude of the sin",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);
    
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_3,
                          "The input phase of the sin",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);
    
    addAttributeToDataSet(devIDInChar,
                          "points",
                          "The number of point that compose the wave",
                          DataType::TYPE_INT32, 
                          DataType::Input);
    
    addAttributeToDataSet(devIDInChar,
                          "frequency",
                          "The frequency of the wave [1-10Mhz]",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);
    addAttributeToDataSet(devIDInChar,
                          "bias",
                          "The bias of the wave",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);
    
    addAttributeToDataSet(devIDInChar,
                          "gain",
                          "The gain of the wave",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);
    
    addAttributeToDataSet(devIDInChar,
                          "phase",
                          "The phase of the wave",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);
    
    addAttributeToDataSet(devIDInChar,
                          "gain_noise",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);
    
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void WorkerCU::init(CDataWrapper *newConfiguration) throw(CException) {
    LAPP_ << "init WorkerCU";
    
    initTime = steady_clock::now();
    lastExecutionTime = steady_clock::now();
    numberOfResponse = 0;
    curAltitude = 1;
    srand((unsigned)time(0));
    PI = acos((long double) -1);
    messageID = 0;
    sinevalue = NULL;
    points = 0;
    freq = 1.0;
    gain = 1.0;
    phase = 0.0;
    bias = 0.0;
    gainNoise = 0;
}

/*
 Execute the Control Unit work
 */
void WorkerCU::run(const string& deviceID) throw(CException) {
    const char *devIDInChar = _deviceID.c_str();
    
    //get new data wrapper instance filled
    //with mandatory data
    CDataWrapper *acquiredData = getNewDataWrapperForKey(devIDInChar);
    if(!acquiredData) return;
    
    //put the messageID for test the lost of package
    acquiredData->addInt32Value("id", ++messageID);
    computeWave(acquiredData);
    //adding some interesting random data

    //submit acquired data
    pushDataSetForKey(devIDInChar, acquiredData);
    
}

void WorkerCU::computeWave(CDataWrapper *acquiredData) {
    if(sinevalue == NULL) return;
    double interval = (2*PI)/points;
    boost::mutex::scoped_lock lock(pointChangeMutex);
    for(int i=0; i<points; i++){
        sinevalue[i] = (gain*sin((interval*i)+phase)+(((double)randInt()/(double)100)*gainNoise)+bias);
    }
    acquiredData->addBinaryValue("sinWave", (char*)sinevalue, sizeof(double)*points);
}


/*
 Execute the Control Unit work
 */
void WorkerCU::stop(const string& deviceID) throw(CException) {
    LAPP_ << "stop WorkerCU for device " << deviceID;
}

/*
 Deinit the Control Unit
 */
void WorkerCU::deinit(const string& deviceID) throw(CException) {
    LAPP_ << "deinit WorkerCU for device " << deviceID;
    if(sinevalue){
        free(sinevalue);
    }
}

/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* WorkerCU::setDatasetAttribute(CDataWrapper *datasetAttrbiuteValue, bool& detachParam) throw (CException) {
    if(!datasetAttrbiuteValue 
       || !datasetAttrbiuteValue->hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) return NULL;
    
    string id = datasetAttrbiuteValue->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    if(datasetAttrbiuteValue->hasKey(DS_ELEMENT_2)){
        curAltitude = datasetAttrbiuteValue->getDoubleValue(DS_ELEMENT_2);
        if(curAltitude < 1) curAltitude = 1;
    }
    
    if(datasetAttrbiuteValue->hasKey(DS_ELEMENT_3)){
        double cur = datasetAttrbiuteValue->getDoubleValue(DS_ELEMENT_3);
        if(cur < 1) cur = 1;
        curPhasePeriod = cur * 1000000;
    }
    
    if(datasetAttrbiuteValue->hasKey("points")){
        boost::mutex::scoped_lock lock(pointChangeMutex);
        int32_t newPoints = datasetAttrbiuteValue->getInt32Value("points");
        if(newPoints < 1) newPoints = 0;
        
        if(!newPoints){
            if(sinevalue){
                free(sinevalue);
                sinevalue = NULL;  
            }
        }else{
            double* tmpPtr = (double*)realloc(sinevalue, sizeof(double) * newPoints);
            if(tmpPtr) {
                sinevalue = tmpPtr;
                memset(sinevalue, 0, points);
            }else{
                //memory can't be enlarged so pointer ramin the same
                //so all remain unchanged
                newPoints = points;
            }
        }
        points = newPoints;
    }
    
    if(datasetAttrbiuteValue->hasKey("freq")){
        freq = datasetAttrbiuteValue->getDoubleValue("freq");
    }
    
    if(datasetAttrbiuteValue->hasKey("gain")){
        gain = datasetAttrbiuteValue->getDoubleValue("gain");
    }
    
    if(datasetAttrbiuteValue->hasKey("phase")){
        phase = datasetAttrbiuteValue->getDoubleValue("phase");
    }
    
    if(datasetAttrbiuteValue->hasKey("bias")){
        bias = datasetAttrbiuteValue->getDoubleValue("bias");
    }
    if(datasetAttrbiuteValue->hasKey("gain_noise")){
        gainNoise = datasetAttrbiuteValue->getDoubleValue("gain_noise");
    }
    
    
    return NULL;
}

/*
 Event for update some CU configuration
 */
CDataWrapper* WorkerCU::updateConfiguration(CDataWrapper *newConfiguration, bool& detachParam) throw (CException) {
    LAPP_ << "updateConfiguration  WorkerCU";
    AbstractControlUnit::updateConfiguration(newConfiguration, detachParam);
    return NULL;
}

/*
 Test Action Handler
 */
CDataWrapper* WorkerCU::actionTestOne(CDataWrapper *actionParam, bool& detachParam) {
    CDataWrapper *result = new CDataWrapper();
    result->addStringValue("result_key", "result_key_value");
    return result;
}

/*
 Test Action Handler
 */
CDataWrapper* WorkerCU::resetStatistic(CDataWrapper *actionParam, bool& detachParam) {
    LAPP_ << "resetStatistic in WorkerCU called from rpc";
    numberOfResponse = 0;
    return NULL;
}

/*
 Test Action Handler
 */
CDataWrapper* WorkerCU::actionTestTwo(CDataWrapper *actionParam, bool& detachParam) {
    LAPP_ << "resetStatistic in WorkerCU called from rpc";
    if(actionParam->hasKey(ACTION_TWO_PARAM_NAME)){
        int32_t sleepTime =  actionParam->getInt32Value(ACTION_TWO_PARAM_NAME);
        
        LAPP_ << "param for actionTestTwo with value:" << sleepTime;
        LAPP_ << "let this thread to waith "<< sleepTime << " usec";
        boost::this_thread::sleep(boost::posix_time::microseconds(sleepTime));
    } else {
        LAPP_ << "No param received for action actionTestTwo";
    }
    return NULL;
}
