/*
 *	StateFlagCatalogTest.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/07/16 INFN, National Institute of Nuclear Physics
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

#include "StateFlagCatalogTest.h"

#include <chaos/common/status_manager/StatusFlagCatalogSDWrapper.h>

#include <boost/foreach.hpp>

using namespace chaos::common::utility::test;
using namespace chaos::common::status_manager;

StatusFlagCatalogTest::StatusFlagCatalogTest():
catalog_a("catalog_a"),
catalog_b("catalog_b"){
    
}

StatusFlagCatalogTest::~StatusFlagCatalogTest() {
    
}

bool StatusFlagCatalogTest::test() {
    boost::shared_ptr<StatusFlag> flag;
    boost::shared_ptr<StatusFlag> flag_powersupply_state;
    boost::shared_ptr<StatusFlag> flag_serial_comunication;
    boost::shared_ptr<StatusFlag> flag_serial_port;
    
    flag_powersupply_state.reset(new StatusFlag("powersupply_state",
                                                      "Represent the state of internal power supply"));
    flag_powersupply_state->addLevel(StateLevel(0, "Off", "Powersupply is off", StatusFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(1, "Standby", "Power supply can't erogate current", StatusFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(2, "Operational", "Powersupply erogate current", StatusFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(3, "faulty", "Powersupply is in faulty", StatusFlagServerityWarning));
    catalog_a.addFlag(flag_powersupply_state);
    
    flag_serial_comunication.reset(new StatusFlag("serial_comunication",
                        "Represente the state of the comunication over the serial port"));
    flag_serial_comunication->addLevel(StateLevel(0, "Working", "Comunication are OK!", StatusFlagServerityRegular));
    flag_serial_comunication->addLevel(StateLevel(1, "Timeout", "No answqer over serial port", StatusFlagServerityWarning));
    catalog_a.addFlag(flag_serial_comunication);
    
    flag_serial_port.reset(new StatusFlag("serial_port_state",
                              "Represente the state of the serial port"));
    flag_serial_port->addLevel(StateLevel(0, "Working", "Serial porto working", StatusFlagServerityRegular));
    flag_serial_port->addLevel(StateLevel(1, "KernelError", "Comunication problem using serial port", StatusFlagServerityCritical));
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
    VectorStatusFlag found_flag_for_severity;
    
    catalog_a.getFlagsForSeverity(StatusFlagServerityRegular, found_flag_for_severity);
    std::cout << "catalog_a StatusFlagServerityRegular"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StatusFlag> flag, found_flag_for_severity ){std::cout << flag->getName() << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_a.getFlagsForSeverity(StatusFlagServerityWarning, found_flag_for_severity);
    std::cout << "catalog_a StatusFlagServerityWarning"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StatusFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_a.getFlagsForSeverity(StatusFlagServerityCritical, found_flag_for_severity);
    std::cout << "catalog_a StatusFlagServerityCritical"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StatusFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_b.getFlagsForSeverity(StatusFlagServerityRegular, found_flag_for_severity);
    std::cout << "catalog_b StatusFlagServerityRegular"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StatusFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_b.getFlagsForSeverity(StatusFlagServerityWarning, found_flag_for_severity);
    std::cout << "catalog_b StatusFlagServerityWarning"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StatusFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    found_flag_for_severity.clear();
    catalog_b.getFlagsForSeverity(StatusFlagServerityCritical, found_flag_for_severity);
    std::cout << "catalog_b StatusFlagServerityCritical"<<std::endl;
    BOOST_FOREACH(boost::shared_ptr<StatusFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
    
    //print serialization
    StatusFlagCatalogSDWrapper sfcsdw;
    sfcsdw.dataWrapped() = catalog_b;
    std::cout << sfcsdw.serialize()->getJSONString() << std::endl;
    return true;
}
