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

using namespace chaos;

#define SIMULATED_DEVICE_ID     "SIMULATED_DEVICE_ID"
#define DS_ELEMENT_1            "intValue_1"
#define DS_ELEMENT_2            "intValue_2"
#define DS_ELEMENT_3            "byteValue"
#define DS_ELEMENT_4            "stringaValue"

#define TEST_BUFFER_DIM         100
#define CU_DELAY_FROM_TASKS     1000000 //1Sec
#define ACTION_TWO_PARAM_NAME   "actionTestTwo_paramName"

WorkerCU::WorkerCU():AbstractControlUnit(),rng((const uint_fast32_t) time(0) ),one_to_six( -10000, 10000 ),randInt(rng, one_to_six) {
    //first we make some write
     _deviceID.assign(SIMULATED_DEVICE_ID);
    cuName = "WORKER_CU";
    writeRead = false;
    numberOfResponse = 0;
}

/*
 Construct a new CU with an identifier
 */
WorkerCU::WorkerCU(string &customDeviceID):rng((const uint_fast32_t) time(0) ),one_to_six( -10000, 10000 ),randInt(rng, one_to_six){
    _deviceID = customDeviceID;
    cuName = "WORKER_CU";
    writeRead = false;
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
    cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_NAME, "WORKER_CU");
    cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION, "This is a beautifull CU");
    cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_CLASS, "HW1-CLASS1");
    //cuSetup.addInt32Value(CUDefinitionKey::CS_CM_CU_AUTOSTART, 1);

    
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
                          "describe the element 1 of the dataset",
                          DataType::TYPE_INT32, 
                          DataType::Output);
    
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_2,
                          "describe the element 2 of the dataset",
                          DataType::TYPE_INT32, 
                          DataType::Bidirectional);
    
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_3,
                          "describe the element 3 of the dataset",
                          DataType::TYPE_BYTEARRAY, 
                          DataType::Output);
    
    
    addAttributeToDataSet(devIDInChar,
                          DS_ELEMENT_4,
                          "describe the element 4 of the dataset",
                          DataType::TYPE_STRING, 
                          DataType::Input);
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void WorkerCU::init(CDataWrapper *newConfiguration) throw(CException) {
    LAPP_ << "init WorkerCU";
    lastExecutionTime = boost::chrono::steady_clock::now();
}

/*
 Execute the Control Unit work
 */
void WorkerCU::run() throw(CException) {
    LAPP_ << "run WorkerCU";
    auto_ptr<SerializationBuffer> jsonResult;
    const char *devIDInChar = _deviceID.c_str();
    string jsonString;
    string bufferHexRepresentation;

    currentExecutionTime = steady_clock::now();
    LAPP_ << "Time beetwen last call(msec):" << (currentExecutionTime-lastExecutionTime);
    lastExecutionTime = currentExecutionTime;

        //get new data wrapper instance filled
        //with mandatory data
    CDataWrapper *acquiredData = NULL;
    
    if(writeRead){
        //int bufferLen = 0;
        //const char * charBuff = NULL;
            //get last data
        auto_ptr< ArrayPointer<CDataWrapper> > result(getLastDataSetForKey(devIDInChar));
        
        if(!result->size()) return;
        
            //ge tthe first and only result
        acquiredData = (*result.get())[0];
        
        if(acquiredData){
            //charBuff = acquiredData->getBinaryValue("byteValue", bufferLen);
            //bufferHexRepresentation.assign(toHex(charBuff, bufferLen));
#if DEBUG
            LAPP_ << "readed data " << acquiredData->getJSONString();
#endif
            //LAPP_ << "byte buffer contains=" << bufferHexRepresentation;
        }
    } else  {
            //get new istance for CDataWrapper fille with rigth key
    acquiredData = getNewDataWrapperForKey(devIDInChar);
    if(!acquiredData) return;
    
            //adding some interesting random data 
    acquiredData->addInt32Value("intValue_1", randInt());
    acquiredData->addInt32Value("intValue_2", randInt());
    acquiredData->addDoubleValue("doubleValue_1", 25.12);
            //generate  test byte
        const char * binData = new char[TEST_BUFFER_DIM];
        
       // bufferHexRepresentation.assign(toHex(binData, TEST_BUFFER_DIM));
        acquiredData->addBinaryValue("byteValue", binData, TEST_BUFFER_DIM);
#if DEBUG  
        LAPP_ << "data to be write=" << acquiredData->getJSONString();
#endif
        //LAPP_ << "byte buffer contains=" << bufferHexRepresentation;
        
        delete[] binData;
            //submit acquired data
        pushDataSetForKey(devIDInChar, acquiredData);
        //}
    }
    writeRead = !writeRead;
}

/*
 Execute the Control Unit work
 */
void WorkerCU::stop() throw(CException) {
    LAPP_ << "stop WorkerCU";
}

/*
 Deinit the Control Unit
 */
void WorkerCU::deinit() throw(CException) {
    LAPP_ << "deinit WorkerCU";
}

/*
 Receive the evento for set the dataset input element
 */
CDataWrapper* WorkerCU::setDatasetAttribute(CDataWrapper*) throw (CException) {
    return NULL;
}

/*
 Event for update some CU configuration
 */
CDataWrapper* WorkerCU::updateConfiguration(CDataWrapper *newConfiguration) throw (CException) {
    LAPP_ << "updateConfiguration  WorkerCU";
    AbstractControlUnit::updateConfiguration(newConfiguration);
    return NULL;
}

/*
 Test Action Handler
 */
CDataWrapper* WorkerCU::actionTestOne(CDataWrapper *actionParam) {
    CDataWrapper *result = new CDataWrapper();
    result->addStringValue("result_key", "result_key_value");
    result->addInt64Value("result_key_int", ++numberOfResponse);
    return result;
}

/*
 Test Action Handler
 */
CDataWrapper* WorkerCU::resetStatistic(CDataWrapper *actionParam) {
    LAPP_ << "resetStatistic in WorkerCU called from rpc";
    numberOfResponse = 0;
    return NULL;
}

/*
 Test Action Handler
 */
CDataWrapper* WorkerCU::actionTestTwo(CDataWrapper *actionParam) {
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
