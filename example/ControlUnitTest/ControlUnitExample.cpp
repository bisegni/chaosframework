//
//  ControlUnitTest.cpp
//  ChaosFrameworkTest
//
//  Created by Claudio Bisegni on 13/02/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include <iostream>
#include <string>

#include "WorkerCU.h"
#include <chaos/common/cconstants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>

/*! \page page_example_cue ChaosCUToolkit Example
 *  \section page_example_cue_sec An basic usage for the ChaosCUToolkit package
 *  
 *  \subsection page_example_cue_sec_sub1 Toolkit usage
 *  ChaosCUToolkit has private constructor so it can be used only using singleton pattern,
 *  the only way to get unique isntance is; ChaosCUToolkit::getInstance(). So the first call of
 *  getInstance method will rpovide the CUToolkit and Common layer initial setup.
 *  \snippet example/ControlUnitTest/ControlUnitExample.cpp Custom Option
 *
 *  Then it must be initialized, in this method can be passed the main argument
 *  of a c or c++ programm
 *  \snippet example/ControlUnitTest/ControlUnitExample.cpp CUTOOLKIT Init
 *
 *  At this point the custom implementation af a control unit cab be added to framework
 *  \snippet example/ControlUnitTest/ControlUnitExample.cpp Adding the CustomControlUnit
 *
 *  At this step the framework can be started, it will perform all needs, comunicate with
 *  Metadata Server and all chaos workflow will start. The start method call will block the main execution
 *  until the chaos workflow of this isnstance need to be online
 *  \snippet example/ControlUnitTest/ControlUnitExample.cpp Starting the Framework
 */
using namespace std;
using namespace chaos;

#define OPT_CUSTOM_DEVICE_ID "custom-device-id"

int main (int argc, const char* argv[] )
{
   
    //! [Custom Option]
    ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_CUSTOM_DEVICE_ID, po::value<string>(), "Custom device identification string");
    //! [Custom Option]
    
    //! [CUTOOLKIT Init]
    ChaosCUToolkit::getInstance()->init(argc, argv);
    //! [CUTOOLKIT Init]
    
    //! [Adding the CustomControlUnit]
    if(ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CUSTOM_DEVICE_ID)){
        string customdeviceID = ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_CUSTOM_DEVICE_ID);
        ChaosCUToolkit::getInstance()->addControlUnit(new WorkerCU(customdeviceID));
    }else{
        ChaosCUToolkit::getInstance()->addControlUnit(new WorkerCU());
    }
    //! [Adding the CustomControlUnit]
    
    //! [Starting the Framework]
    ChaosCUToolkit::getInstance()->start();
    //! [Starting the Framework]
    return 0;
}
