/*	
 *	SWAlimCU.cpp
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

#include "SWAlimCU.h"
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

#define SIMULATED_DEVICE_ID     "SWAlim"
#define DS_ELEMENT_1            "current"
#define DS_ELEMENT_2            "voltage"

#define TEST_BUFFER_DIM         100
#define CU_DELAY_FROM_TASKS     1000000 //1Sec
#define ACTION_TWO_PARAM_NAME   "actionTestTwo_paramName"



SWAlimCU::SWAlimCU():AbstractControlUnit(),rng((const uint_fast32_t) time(0) ),one_to_hundred( -100, 100 ),randInt(rng, one_to_hundred) {
    //first we make some write
    _deviceID.assign(SIMULATED_DEVICE_ID);
    cuName = "SWAlim_CU";
    numberOfResponse = 0;
}

/*
 Construct a new CU with an identifier
 */
SWAlimCU::SWAlimCU(string &customDeviceID):rng((const uint_fast32_t) time(0) ),one_to_hundred( -100, 100 ),randInt(rng, one_to_hundred){
    _deviceID = customDeviceID;
    cuName = "SWAlim_CU";
    numberOfResponse = 0;
    tcpChan = new TcpChannel("LocalTcp");
    prot = new ModbusProtocol("modbus");
    myalim = new SWAlim(_deviceID.c_str(),"localhost:8083");
    LAPP_ << "Created Channel:"<<tcpChan->getName()<<endl<<"Created Protocol:"<<prot->getName()<<endl<<"Created device:"<<myalim->getName()<<endl;

}

/*
 Destructor a new CU with an identifier
 */
SWAlimCU::~SWAlimCU() {
    
}

/*
 Return the default configuration
 */
void SWAlimCU::defineActionAndDataset() throw(CException) {
    //set the base information
    const char *devIDInChar = _deviceID.c_str();
    
    
    //set the default delay for the CU
    setDefaultScheduleDelay(CU_DELAY_FROM_TASKS);
    
    //add managed device di
    addDeviceId(_deviceID);
    
    //add custom action
    AbstActionDescShrPtr  
    actionDescription = addActionDescritionInstance<SWAlimCU>(this, 
                                                              &SWAlimCU::actionTestOne, 
                                                              "actionTestOne", 
                                                              "comandTestOne this action will do some beautifull things!");
    
    actionDescription = addActionDescritionInstance<SWAlimCU>(this, 
                                                              &SWAlimCU::resetStatistic, 
                                                              "resetStatistic", 
                                                              "resetStatistic this action will reset  all cu statistic!");
    
    actionDescription = addActionDescritionInstance<SWAlimCU>(this, 
                                                              &SWAlimCU::actionTestTwo, 
                                                              "actionTestTwo", 
                                                              "comandTestTwo, this action will do some beautifull things!");
    
    //add param to second action
    actionDescription->addParam(ACTION_TWO_PARAM_NAME, 
                                DataType::TYPE_INT32, 
                                "integer 32bit action param description for testing purpose");
    

    //setup the dataset
    addAttributeToDataSet(devIDInChar,
                          "Current",
                          "SWAlim current",
                          DataType::TYPE_INT32,
                          DataType::Output);
    
    addAttributeToDataSet(devIDInChar,
                          "Voltage",
                          "SWAlim voltage",
                          DataType::TYPE_INT32,
                          DataType::Output);
    
    addInputInt32AttributeToDataSet<SWAlimCU>(devIDInChar,
                                              "Setcurrent",
                                              "Set The current",
                                              this,
                                              &SWAlimCU::setCurrent);

}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void SWAlimCU::init(const string& deviceID) throw(CException) {
    LAPP_ << "init SWAlimCU";
    initTime = steady_clock::now();
    lastExecutionTime = steady_clock::now();
    numberOfResponse = 0;
    messageID = 0;
    prot->attach(tcpChan);
    myalim->attach(prot);
    if(myalim->init()!=0)
        throw new CException(0,"cannot init SWAlim",__FUNCTION__);
}

/*
 Execute the Control Unit work
 */
void SWAlimCU::run(const string& deviceID) throw(CException) {
    const char *devIDInChar = _deviceID.c_str();
    int current,voltage;
    //get new data wrapper instance filled
    //with mandatory data
    CDataWrapper *acquiredData = getNewDataWrapperForKey(devIDInChar);
    if(!acquiredData) return;
    myalim->readCurrent(current);
    myalim->readVoltage(voltage);
    //put the messageID for test the lost of package
    acquiredData->addInt32Value("id", ++messageID);
    acquiredData->addInt32Value("Current", current);
    acquiredData->addInt32Value("Voltage", voltage);
    //adding some interesting random data
    LAPP_<<"Current:"<<current<<endl<<"Voltage:"<<voltage<<endl;
    
    //submit acquired data
    pushDataSetForKey(devIDInChar, acquiredData);
    
}


/*
 Execute the Control Unit work
 */
void SWAlimCU::stop(const string& deviceID) throw(CException) {
    LAPP_ << "stop SWAlimCU for device " << deviceID;
}

/*
 Deinit the Control Unit
 */
void SWAlimCU::deinit(const string& deviceID) throw(CException) {
    LAPP_ << "deinit SWAlimCU for device " << deviceID;
    myalim->deinit();
}

/*
 */
void SWAlimCU::setCurrent(const std::string& deviceID, const int32_t& current) {
    myalim->setCurrent(current);
}

/*
 Test Action Handler
 */
CDataWrapper* SWAlimCU::actionTestOne(CDataWrapper *actionParam, bool& detachParam) {
    CDataWrapper *result = new CDataWrapper();
    result->addStringValue("result_key", "result_key_value");
    return result;
}

/*
 Test Action Handler
 */
CDataWrapper* SWAlimCU::resetStatistic(CDataWrapper *actionParam, bool& detachParam) {
    LAPP_ << "resetStatistic in SWAlimCU called from rpc";
    numberOfResponse = 0;
    return NULL;
}

/*
 Test Action Handler
 */
CDataWrapper* SWAlimCU::actionTestTwo(CDataWrapper *actionParam, bool& detachParam) {
    LAPP_ << "resetStatistic in SWAlimCU called from rpc";
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
