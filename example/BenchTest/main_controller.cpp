/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include <iostream>
#include <string>
#include <vector>
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <stdio.h>
#include <chaos/common/bson/bson.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
/*! \page page_example_uiex1 ChaosUIToolkit Example
 \section page_example_uiex1_sec A basic usage for the ChaosUIToolkit package
 
 \subsection page_example_uiex1_sec_sub1 Toolkit usage
 As in the CUToolkit, a developer can add a custom startup param
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Init
 
 this example show how initialize the UIToolkit, acquire a basic channel, used it and deinit the toolkit.
 The UIToolkit is create around singleton pattern and the channel object are self managed by toolkit. So the first thing
 is to initialize the toolkit internal engine:
 
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Init
 
 the argc, argv parameter are the common c and cpp main param.
 
 For comunicate with the chaos resurces, as example the metadataserver, a channel is needed. To instantiate a mds channel
 must be used the LLRpcApi api package. To achieve channel creation can be used the singleton instance for calling the method "getNewMetadataServerChannel"
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit ChannelCreation
 The returned pointer to MDSMessageChannel is interally managed, there is no need to deallocate it by and. When a channel is no more neede, the
 deallocation api must be called.
 
 At this point a request can be sent to metadata server and we must wait for the answer. For get all active devices we can use the method "getAllDeviceID"
 that get as param a vector of string, that wil be filled with all device id found
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp Datapack sent
 
 When all operation are finisched the UIToolkit ca be deinitialized, this operation will clean all pendi operation and hannel deallocation
 \snippet example/UIToolkitCMDLineExample/UIToolkitCMDLineExample.cpp UIToolkit Deinit
 */
using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace bson;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::date_time;

#include <vector>
#include <string>
#include "config.h"
#include "MainOrkestrator.h"

inline ptime utcToLocalPTime(ptime utcPTime){
	c_local_adjustor<ptime> utcToLocalAdjustor;
	ptime t11 = utcToLocalAdjustor.utc_to_local(utcPTime);
	return t11;
}

int main (int argc, char* argv[] )
{
    try {
        std::vector< std::string > names;
        std::vector< double > referements;
        std::vector< uint64_t > simulated_speed;
        
        string tmpDeviceID("bench_reactor");
        
        //initial state value
        vector<double> stateOption;
        posix_time::time_duration currentTime;
        
        //! [UIToolkit Attribute Init]
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(REACTOR_NAMES, po::value< std::vector< std::string > >()->multitoken(), "The id (and implicit the number) of the rectors");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(REACTOR_REFEREMENT_VALUE, po::value< std::vector< double > >()->multitoken(), "The name (and implicit the number) of the rectors");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(SIMULATION_SPEED_USEC, po::value< std::vector< uint64_t > >()->multitoken(), "The speed of simulation of the rectors");
        //! [UIToolkit Attribute Init]
        
        //! [UIToolkit Init]
        ChaosUIToolkit::getInstance()->init(argc, argv);
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(REACTOR_NAMES)){
            names = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption< std::vector< std::string > >(REACTOR_NAMES);
        } else {
            names.push_back(tmpDeviceID);
        }
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(REACTOR_REFEREMENT_VALUE)){
            referements = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption< std::vector< double > >(REACTOR_REFEREMENT_VALUE);
        } else {
            for(int idx = 0; idx < Q; idx++) {
                referements.push_back(0.0);
            }
        }
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(SIMULATION_SPEED_USEC)){
            simulated_speed = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption< std::vector< uint64_t > >(SIMULATION_SPEED_USEC);
        } else {
            simulated_speed.push_back(5000);
        }
        
        if(referements.size() != names.size() * Q) {
            char error[128];
            sprintf ( error, "every reactor identified need to have %d referement", Q );
            throw chaos::CException(0, error, "main");
        }
        if(names.size() != simulated_speed.size()) {
            throw chaos::CException(1, "every reactor identified need to have default speed", "main");
        }
        //! [UIToolkit Init]
        
        //! [Allocate the main orkestrator]
        auto_ptr<MainOrkestrator> ork(new MainOrkestrator(&names, &referements, &simulated_speed));
        ork->init();
        ork->join();
        //! [Allocate the main orkestrator]
        
        //! [UIToolkit Deinit]
        ChaosUIToolkit::getInstance()->deinit();
        //! [UIToolkit Deinit]
    } catch (CException& e) {
        std::cerr<< e.errorDomain << std::endl;
        std::cerr<< e.errorMessage << std::endl;
    }
    return 0;
}
