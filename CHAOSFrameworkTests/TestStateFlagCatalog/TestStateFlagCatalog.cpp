/*
 *	TestStateFlagCatalog.cpp
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by bisegni.
 *
 *    	Copyright 22/08/16 INFN, National Institute of Nuclear Physics
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


#include "TestStateFlagCatalog.h"


#include <chaos/common/state_flag/StateFlagCatalogSDWrapper.h>

#include <boost/foreach.hpp>

using namespace chaos::common::utility::test;
using namespace chaos::common::state_flag;
using namespace chaos::common::alarm;

TestStateFlagCatalog::TestStateFlagCatalog():
alarm_catalog("control_unit_alarm"),
catalog_a("catalog_a"),
catalog_b("catalog_b"){}

TestStateFlagCatalog::~TestStateFlagCatalog() {}

bool TestStateFlagCatalog::test() {
    boost::shared_ptr<StateFlag> flag;
    boost::shared_ptr<StateFlag> flag_powersupply_state;
    boost::shared_ptr<StateFlag> flag_serial_comunication;
    boost::shared_ptr<StateFlag> flag_serial_port;
    
    flag_powersupply_state.reset(new StateFlag("powersupply_state",
                                                "Represent the state of internal power supply"));
    flag_powersupply_state->addLevel(StateLevel(0, "Off", "Powersupply is off", StateFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(1, "Standby", "Power supply can't erogate current", StateFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(2, "Operational", "Powersupply erogate current", StateFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(3, "faulty", "Powersupply is in faulty", StateFlagServerityLow));
    catalog_a.addFlag(flag_powersupply_state);
    
    flag_serial_comunication.reset(new StateFlag("serial_comunication",
                                                  "Represente the state of the comunication over the serial port"));
    flag_serial_comunication->addLevel(StateLevel(0, "Working", "Comunication are OK!", StateFlagServerityRegular));
    flag_serial_comunication->addLevel(StateLevel(1, "Timeout", "No answqer over serial port", StateFlagServerityLow));
    catalog_a.addFlag(flag_serial_comunication);
    
    flag_serial_port.reset(new StateFlag("serial_port_state",
                                          "Represente the state of the serial port"));
    flag_serial_port->addLevel(StateLevel(0, "Working", "Serial porto working", StateFlagServerityRegular));
    flag_serial_port->addLevel(StateLevel(1, "KernelError", "Comunication problem using serial port", StateFlagServeritySevere));
    catalog_b.addFlag(flag_serial_port);
    
    flag = catalog_a.getFlagByName("powersupply_state");
    assert(flag->getName().compare("powersupply_state") == 0);
    flag = catalog_a.getFlagByName("serial_comunication");
    assert(flag->getName().compare("serial_comunication") == 0);
    flag = catalog_b.getFlagByName("serial_port_state");
    assert(flag->getName().compare("serial_port_state") == 0);
    
    flag = catalog_a.getFlagByOrderedID(0);
    assert(flag->getName().compare("powersupply_state") == 0);
    flag = catalog_a.getFlagByOrderedID(1);
    assert(flag->getName().compare("serial_comunication") == 0);
    flag = catalog_b.getFlagByOrderedID(0);
    assert(flag->getName().compare("serial_port_state") == 0);
    
    //cataog union test
    catalog_b.appendCatalog(catalog_a);
    flag = catalog_b.getFlagByOrderedID(0);
    assert(flag->getName().compare("serial_port_state") == 0);
    flag = catalog_b.getFlagByOrderedID(1);
    assert(flag->getName().compare("powersupply_state") == 0);
    flag = catalog_b.getFlagByOrderedID(2);
    assert(flag->getName().compare("serial_comunication") == 0);
    
    flag = catalog_b.getFlagByName("serial_port_state");
    assert(flag->getName().compare("serial_port_state") == 0);
    flag = catalog_b.getFlagByName("catalog_a/powersupply_state");
    assert(flag->getName().compare("powersupply_state") == 0);
    flag = catalog_b.getFlagByName("catalog_a/serial_comunication");
    assert(flag->getName().compare("serial_comunication") == 0);
    
    //change some level
    flag_powersupply_state->setCurrentLevel(3);
    flag_serial_port->setCurrentLevel(1);
    VectorStateFlag found_flag_for_severity;
    
    catalog_a.getFlagsForSeverity(StateFlagServerityRegular, found_flag_for_severity);
    std::cout << "catalog_a StateFlagServerityRegular"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName() << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_a.getFlagsForSeverity(StateFlagServerityLow, found_flag_for_severity);
    std::cout << "catalog_a StateFlagServerityWarning"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_a.getFlagsForSeverity(StateFlagServeritySevere, found_flag_for_severity);
    std::cout << "catalog_a StateFlagServerityCritical"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_b.getFlagsForSeverity(StateFlagServerityRegular, found_flag_for_severity);
    std::cout << "catalog_b StateFlagServerityRegular"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_b.getFlagsForSeverity(StateFlagServerityLow, found_flag_for_severity);
    std::cout << "catalog_b StateFlagServerityWarning"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_b.getFlagsForSeverity(StateFlagServerityLow, found_flag_for_severity);
    std::cout << "catalog_b StateFlagServerityCritical"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    //print serialization
    StateFlagCatalogSDWrapper sfcsdw;
    sfcsdw.dataWrapped() = catalog_b;
    std::cout << sfcsdw.serialize()->getJSONString() << std::endl;
    return true;
}

