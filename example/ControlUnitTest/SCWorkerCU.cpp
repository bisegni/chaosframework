//
//  SCWorkerCU.cpp
//  ControlUnitTest
//
//  Created by Claudio Bisegni on 7/20/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "SCWorkerCU.h"
#include "SinWaveCommand.h"

/*
 Construct a new CU with an identifier
 */
SCWorkerCU::SCWorkerCU(string &customDeviceID) {
    _deviceID = customDeviceID;
    cuName = "WORKER_CU";
}

/*
 Return the default configuration
 */
void SCWorkerCU::defineActionAndDataset() throw(CException) {
    //set the base information
    const char *devIDInChar = _deviceID.c_str();
    cuName = "SIN_CU";
    //cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION, "This is a beautifull CU");
    
    //add managed device di
    setDeviceID(_deviceID);
    
    //install a command
    installCommand<SinWaveCommand>("sinwave_base");
    
    //set it has default
    setDefaultCommand("sinwave_base");
    
    //setup the dataset
    addAttributeToDataSet(devIDInChar,
                          "sinWave",
                          "The sin waveform",
                          DataType::TYPE_BYTEARRAY,
                          DataType::Output,
                          10000);
    

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