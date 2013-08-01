//
//  SCWorkerCU.cpp
//  ControlUnitTest
//
//  Created by Claudio Bisegni on 7/20/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "SCWorkerCU.h"
#include "SinWaveCommand.h"
namespace cucs = chaos::cu::control_manager::slow_command;
/*
 Construct a new CU with an identifier
 */
SCWorkerCU::SCWorkerCU(string &customDeviceID) {
    _deviceID = customDeviceID;
}

/*
 Return the default configuration
 */
void SCWorkerCU::defineActionAndDataset() throw(CException) {
    //set the base information
    RangeValueInfo rangeInfoTemp;
    //cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION, "This is a beautifull CU");
    
    //add managed device di
    setDeviceID(_deviceID);
    
    //install a command
    installCommand<SinWaveCommand>("sinwave_base");
    
    //set it has default
   // setDefaultCommand("sinwave_base");
    
    //setup the dataset
    addAttributeToDataSet("sinWave",
                          "The sin waveform",
                          DataType::TYPE_BYTEARRAY,
                          DataType::Output,
                          10000);
    
    addAttributeToDataSet("points",
                          "The number of point that compose the wave",
                          DataType::TYPE_INT32,
                          DataType::Input);
    rangeInfoTemp.defaultValue = "30";
    rangeInfoTemp.maxRange = "";
    rangeInfoTemp.minRange = "30";
    setAttributeRangeValueInfo("points", rangeInfoTemp);
    
    addAttributeToDataSet("frequency",
                          "The frequency of the wave [1-10Mhz]",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("bias",
                          "The bias of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("phase",
                          "The phase of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("gain",
                          "The gain of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);

    addAttributeToDataSet("gain_noise",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);

}

void SCWorkerCU::defineSharedVariable() {
    bool quit = false;
    addSharedVariable("quit", 1, chaos::DataType::TYPE_BOOLEAN);
    setSharedVariableValue("quit", &quit, sizeof(bool));
}