//
//  main.cpp
//  SWAlim
//
//  Created by andrea michelotti on 5/9/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "SWAlimCU.h"
#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>

#include <iostream>
#include <string>
using namespace std;
using namespace chaos;
using namespace chaos::cu;
#define OPT_CUSTOM_DEVICE_ID_A "device_a"
#define OPT_CUSTOM_DEVICE_ID_B "device_b"

int main(int argc, char * argv[])
{

    string tmpDeviceID;
    //! [Custom Option]
    try {
        ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_CUSTOM_DEVICE_ID_A, po::value<string>(), "Device A identification string");
        ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_CUSTOM_DEVICE_ID_B, po::value<string>(), "Device B identification string");
        //! [Custom Option]
        
        //! [CUTOOLKIT Init]
        ChaosCUToolkit::getInstance()->init(argc, argv);
        //! [CUTOOLKIT Init]
        
        //! [Adding the CustomControlUnit]
        if(ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CUSTOM_DEVICE_ID_A)){
            tmpDeviceID = ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_CUSTOM_DEVICE_ID_A);
            ChaosCUToolkit::getInstance()->addControlUnit(new SWAlimCU(tmpDeviceID));
        }
        
        if(ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CUSTOM_DEVICE_ID_B)){
            tmpDeviceID = ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_CUSTOM_DEVICE_ID_B);
            ChaosCUToolkit::getInstance()->addControlUnit(new SWAlimCU(tmpDeviceID));
        }
        //! [Adding the CustomControlUnit]
        
        //! [Starting the Framework]
        ChaosCUToolkit::getInstance()->start();
        //! [Starting the Framework]
    } catch (CException& e) {
        cerr<<"Exception::"<<endl;
        std::cerr<< "in:"<<e.errorDomain << std::endl;
        std::cerr<< "cause:"<<e.errorMessage << std::endl;
    } catch (program_options::error &e){
        cerr << "Unable to parse command line: " << e.what() << endl;
    } catch (...){
        cerr << "unexpected exception caught.. " << endl;
    }
    
    return 0;
}

