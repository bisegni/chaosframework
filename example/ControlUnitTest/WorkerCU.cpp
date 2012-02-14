    //
    //  File.cpp
    //  ChaosFramework
    //
    //  Created by bisegni on 23/06/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include <boost/thread.hpp>

#include "WorkerCU.h"
#include <chaos/common/global.h>
#include <chaos/common/cconstants.h>
#include <chaos/common/bson/bson.h>
#include <chaos/common/bson/util/hex.h>
#include <chaos/common/action/ActionDescriptor.h>
#include <cmath>
using namespace chaos;

#define SIMULATED_DEVICE_ID     "SIN_DEVICE"
#define DS_ELEMENT_1            "sinOutput"
#define DS_ELEMENT_2            "sinPhase"

#define TEST_BUFFER_DIM         100
#define CU_DELAY_FROM_TASKS     1000000 //1Sec
#define ACTION_TWO_PARAM_NAME   "actionTestTwo_paramName"

WorkerCU::WorkerCU():AbstractControlUnit(),rng((const uint_fast32_t) time(0) ),one_to_six( 1, 100 ),randInt(rng, one_to_six) {
        //first we make some write
    _deviceID.assign(SIMULATED_DEVICE_ID);
    cuName = "WORKER_CU";
    numberOfResponse = 0;
}

/*
 Construct a new CU with an identifier
 */
WorkerCU::WorkerCU(string &customDeviceID):rng((const uint_fast32_t) time(0) ),one_to_six( 1, 100 ),randInt(rng, one_to_six){
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
    
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_2,
                          "The input phase of the sin",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);
 /*   
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_3,
                          "describe the element 3 of the dataset",
                          DataType::TYPE_BYTEARRAY, 
                          DataType::Output);
    
    
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_4,
                          "describe the element 4 of the dataset",
                          DataType::TYPE_DOUBLE, 
                          DataType::Input);*/
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void WorkerCU::init(CDataWrapper *newConfiguration) throw(CException) {
    LAPP_ << "init WorkerCU";
    lastExecutionTime = boost::chrono::steady_clock::now();
    numberOfResponse = 0;
}

/*
 Execute the Control Unit work
 */
void WorkerCU::run(const string& deviceID) throw(CException) {
    LAPP_ << "run WorkerCU for device" << deviceID;
    auto_ptr<SerializationBuffer> jsonResult;
    const char *devIDInChar = _deviceID.c_str();
    string jsonString;
    string bufferHexRepresentation;
    
    currentExecutionTime = steady_clock::now();
    LAPP_ << "Time beetwen last call(msec):" << (currentExecutionTime-lastExecutionTime);
    lastExecutionTime = currentExecutionTime;
    
        //get new data wrapper instance filled
        //with mandatory data
    CDataWrapper *acquiredData = getNewDataWrapperForKey(devIDInChar);
    if(!acquiredData) return;
    
        //adding some interesting random data 
    numberOfResponse+=0.1;
    double_t sinValue = std::sin(numberOfResponse);
    LAPP_ << "Sin Value:" << sinValue;
    acquiredData->addDoubleValue(DS_ELEMENT_1, sinValue);
    //acquiredData->addInt32Value("intValue_2", randInt());
        //generate  test byte
    //const char * binData = new char[TEST_BUFFER_DIM];
    
        // bufferHexRepresentation.assign(toHex(binData, TEST_BUFFER_DIM));
    //acquiredData->addBinaryValue("byteValue", binData, TEST_BUFFER_DIM);
    //acquiredData->addDoubleValue("doubleValue_1", 25.12);
    //delete[] binData;
        //submit acquired data
    pushDataSetForKey(devIDInChar, acquiredData);
    
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
}

/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* WorkerCU::setDatasetAttribute(CDataWrapper *datasetAttrbiuteValue, bool& detachParam) throw (CException) {
    if(datasetAttrbiuteValue) LAPP_ << "received message" << datasetAttrbiuteValue->getJSONString();
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
    result->addInt64Value("result_key_int", ++numberOfResponse);
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
