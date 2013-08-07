/*	
 *	ControlUnitTest.cpp
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

#include "RTWorkerCU.h"
#include "SCWorkerCU.h"
#include "Sl7TcpDriver.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>

#include <iostream>
#include <string>

/*! \page page_example_cue ChaosCUToolkit Example
 *  \section page_example_cue_sec An basic usage for the ChaosCUToolkit package
 *  
 *  \subsection page_example_cue_sec_sub1 Toolkit usage
 *  ChaosCUToolkit has private constructor so it can be used only using singleton pattern,
 *  the only way to get unique isntance is; ChaosCUToolkit::getInstance(). So the first call of
 *  getInstance method will provide the CUToolkit and Common layer initial setup.
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
using namespace chaos::cu;

namespace common_plugin = chaos::common::plugin;
namespace common_utility = chaos::common::utility;
namespace cu_driver_manager = chaos::cu::driver_manager;

#define OPT_CUSTOM_DEVICE_ID_A "device_a"
#define OPT_CUSTOM_DEVICE_ID_B "device_b"


int main (int argc, char* argv[] )
{
    string tmpDeviceID;
    //! [Custom Option]
    try {
    ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_CUSTOM_DEVICE_ID_A, po::value<string>(), "Real Time wave simultaion Device A identification string");
    ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_CUSTOM_DEVICE_ID_B, po::value<string>(), "Slow Controlled wave simulation Device B identification string");
    //! [Custom Option]
    
	//! [Driver Registration]
	MATERIALIZE_INSTANCE_AND_INSPECTOR(Sl7TcpDriver)
	cu_driver_manager::DriverManager::getInstance()->registerDriver(Sl7TcpDriverInstancer, Sl7TcpDriverInspector);
    //! [Driver Registration]
		
    ChaosCUToolkit::getInstance()->init(argc, argv);
    //! [CUTOOLKIT Init]
    
    //! [Adding the CustomControlUnit]
    if(ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CUSTOM_DEVICE_ID_A)){
        tmpDeviceID = ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_CUSTOM_DEVICE_ID_A);
        ChaosCUToolkit::getInstance()->addControlUnit(new RTWorkerCU(tmpDeviceID));
    }
    
    if(ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CUSTOM_DEVICE_ID_B)){
        tmpDeviceID = ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_CUSTOM_DEVICE_ID_B);
        ChaosCUToolkit::getInstance()->addControlUnit(new SCWorkerCU(tmpDeviceID));
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
