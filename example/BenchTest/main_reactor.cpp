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

#include "BenchTestCU.h"
#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>

#include <iostream>
#include <string>

/*! \page page_benchtest_sd Bench Test Simulated Device
 *  \section page_benchtest_sd_sec This represent a simulated device for a bench test in !CHOAS
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

#include <vector>
#include <string>
#include "config.h"
#include "Batch_Reactor.h"

int main (int argc, char* argv[] )
{
    int64_t reactorsNumber = 0;
    std::vector< std::string > names;
    string tmpDeviceID("bench_reactor");
    
    //initial state value
    vector<double> stateOption;
    try {
        //! [Custom Option]
        ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(REACTOR_NAMES, po::value< std::vector< std::string > >()->multitoken(), "The name (and implicit the number) of the rectors");
        ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_START_STATE, po::value< std::vector<double> >()->multitoken(), "Initila condition for the Rector internal state");
        //! [Custom Option]
        
        //! [CUTOOLKIT Init]
        ChaosCUToolkit::getInstance()->init(argc, argv);
        //! [CUTOOLKIT Init]
        
        
        
        
        //! [Adding the CustomControlUnit]
        if(ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(REACTOR_NAMES)){
            names = ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->getOption< std::vector< std::string > >(REACTOR_NAMES);
            reactorsNumber = names.size();
        }
        
        if(ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_START_STATE)){
            stateOption =  ChaosCUToolkit::getInstance()->getGlobalConfigurationInstance()->getOption< std::vector< double > >(OPT_START_STATE);
        } else {
            for (int i = 0; i < N; i++) {
                stateOption.push_back(0.0);
            }
        }
        
        //allocate the reactor isnstance
        if(reactorsNumber >= 1) {
            for (int idx1 = 0; idx1 < reactorsNumber; idx1++) {
                Batch_Reactor *rInstance = new Batch_Reactor();
                for (int idx2 = 0; idx2 < N; idx2++) {
                    rInstance->x[idx2] = stateOption[idx2];
                }
                ChaosCUToolkit::getInstance()->addControlUnit(new BenchTestCU(names[idx1], rInstance));
            }
        } else {
            Batch_Reactor *rInstance = new Batch_Reactor();
            for (int idx = 0; idx < N; idx++) {
                rInstance->x[idx] = stateOption[idx];
            }
            ChaosCUToolkit::getInstance()->addControlUnit(new BenchTestCU(tmpDeviceID, rInstance));
        }
        
        //! [Adding the CustomControlUnit]
        
        //! [Starting the Framework]
        ChaosCUToolkit::getInstance()->start();
        //! [Starting the Framework]
    } catch (CException& e) {
        std::cerr<< e.errorDomain << std::endl;
        std::cerr<< e.errorMessage << std::endl;
    }
    return 0;
}
