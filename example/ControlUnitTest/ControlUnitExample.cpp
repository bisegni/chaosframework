//
//  ControlUnitTest.cpp
//  ControlSystemLibTest
//
//  Created by Claudio Bisegni on 13/02/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include <iostream>
#include <string>

#include "WorkerCU.h"
#include <chaos/common/cconstants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>
#include <chaos/cu_toolkit/ConfigurationManager/ConfigurationManager.h>

/*! \page ChaosCUToolkit_example ChaosCUToolkit Examples
 *  \section sec An basic usage for the ChaosCUToolkit package
 *  
 *  \subsection sec Toolkit usage
 *  First the ChaosCUToolkit class must be created
 *  \snippet example/ControlUnitTest/ControlUnitExample.cpp CUToolkit Instantiation
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
    //! [CUToolkit Instantiation]
    ChaosCUToolkit csl;
    //! [CUToolkit Instantiation]
    
    //! [Custom Option]
    csl.getGlobalConfigurationInstance()->addOption(OPT_CUSTOM_DEVICE_ID, po::value<string>(), "Custom device identification string");
    //! [Custom Option]
    
    //! [CUTOOLKIT Init]
    csl.init(argc, argv);
    //! [CUTOOLKIT Init]
    
    //! [Adding the CustomControlUnit]
    if(csl.getGlobalConfigurationInstance()->hasOption(OPT_CUSTOM_DEVICE_ID)){
        string customdeviceID = csl.getGlobalConfigurationInstance()->getOption<string>(OPT_CUSTOM_DEVICE_ID);
        csl.addControlUnit(new WorkerCU(customdeviceID));
    }else{
        csl.addControlUnit(new WorkerCU());
    }
    //! [Adding the CustomControlUnit]
    
    //! [Starting the Framework]
    csl.start();
    //! [Starting the Framework]
    return 0;
}
