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

#include "TestStateFlagCatalog.h"

#include <chaos/common/state_flag/StateFlagCatalogSDWrapper.h>

#include <boost/foreach.hpp>

using namespace chaos::common::state_flag;
using namespace chaos::common::alarm;

TestStateFlagCatalog::TestStateFlagCatalog():
alarm_catalog("control_unit_alarm"),
catalog_a("catalog_a"),
catalog_b("catalog_b"){}

void TestStateFlagCatalog::SetUp() {
    ChaosSharedPtr<StateFlag> flag_powersupply_state;
    ChaosSharedPtr<StateFlag> flag_serial_comunication;
    ChaosSharedPtr<StateFlag> flag_serial_port;
    
    flag_powersupply_state.reset(new StateFlag("powersupply_state",
                                               "Represent the state of internal power supply"));
    flag_powersupply_state->addLevel(StateLevel(0, "Off", "Powersupply is off", StateFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(1, "Standby", "Power supply can't erogate current", StateFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(2, "Operational", "Powersupply erogate current", StateFlagServerityRegular));
    flag_powersupply_state->addLevel(StateLevel(3, "faulty", "Powersupply is in faulty", StateFlagServeritySevere));
    catalog_a.addFlag(flag_powersupply_state);
    
    flag_serial_comunication.reset(new StateFlag("serial_comunication",
                                                 "Represente the state of the comunication over the serial port"));
    flag_serial_comunication->addLevel(StateLevel(0, "Working", "Comunication are OK!", StateFlagServerityRegular));
    flag_serial_comunication->addLevel(StateLevel(1, "Timeout", "No answqer over serial port", StateFlagServeritySevere));
    catalog_a.addFlag(flag_serial_comunication);
    
    flag_serial_port.reset(new StateFlag("serial_port_state",
                                         "Represente the state of the serial port"));
    flag_serial_port->addLevel(StateLevel(0, "Working", "Serial porto working", StateFlagServerityRegular));
    flag_serial_port->addLevel(StateLevel(1, "KernelError", "Comunication problem using serial port", StateFlagServeritySevere));
    catalog_b.addFlag(flag_serial_port);
}

TEST_F(TestStateFlagCatalog, TestStateFlagCatalog) {
    ChaosSharedPtr<StateFlag> flag;
    
    flag = catalog_a.getFlagByName("powersupply_state");
    ASSERT_STREQ(flag->getName().c_str(),"powersupply_state");
    flag = catalog_a.getFlagByName("serial_comunication");
    ASSERT_STREQ(flag->getName().c_str(),"serial_comunication");
    flag = catalog_b.getFlagByName("serial_port_state");
    ASSERT_STREQ(flag->getName().c_str(),"serial_port_state");
    
    flag = catalog_a.getFlagByOrderedID(0);
    ASSERT_STREQ(flag->getName().c_str(),"powersupply_state");
    flag = catalog_a.getFlagByOrderedID(1);
    ASSERT_STREQ(flag->getName().c_str(),"serial_comunication");
    flag = catalog_b.getFlagByOrderedID(0);
    ASSERT_STREQ(flag->getName().c_str(),"serial_port_state");
    
    //cataog union test
    catalog_b.appendCatalog(catalog_a);
    flag = catalog_b.getFlagByOrderedID(0);
    ASSERT_STREQ(flag->getName().c_str(),"serial_port_state");
    flag = catalog_b.getFlagByOrderedID(1);
    ASSERT_STREQ(flag->getName().c_str(),"powersupply_state");
    flag = catalog_b.getFlagByOrderedID(2);
    ASSERT_STREQ(flag->getName().c_str(),"serial_comunication");
    
    flag = catalog_b.getFlagByName("serial_port_state");
    ASSERT_STREQ(flag->getName().c_str(),"serial_port_state");
    flag = catalog_b.getFlagByName("catalog_a/powersupply_state");
    ASSERT_STREQ(flag->getName().c_str(),"powersupply_state");
    flag = catalog_b.getFlagByName("catalog_a/serial_comunication");
    ASSERT_STREQ(flag->getName().c_str(),"serial_comunication");
    
    //change some level
//    VectorStateFlag found_flag_for_severity;
//    
//    catalog_a.getFlagsForSeverity(StateFlagServerityRegular, found_flag_for_severity);
//    std::cout << "catalog_a StateFlagServerityRegular"<<std::endl;
//    BOOST_FOREACH(ChaosSharedPtr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName() << std::endl;}
//    
//    found_flag_for_severity.clear();
//    catalog_a.getFlagsForSeverity(StateFlagServerityWarning, found_flag_for_severity);
//    std::cout << "catalog_a StateFlagServerityWarning"<<std::endl;
//    BOOST_FOREACH(ChaosSharedPtr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
//    
//    found_flag_for_severity.clear();
//    catalog_a.getFlagsForSeverity(StateFlagServeritySevere, found_flag_for_severity);
//    std::cout << "catalog_a StateFlagServerityCritical"<<std::endl;
//    BOOST_FOREACH(ChaosSharedPtr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
//    
//    found_flag_for_severity.clear();
//    catalog_b.getFlagsForSeverity(StateFlagServerityRegular, found_flag_for_severity);
//    std::cout << "catalog_b StateFlagServerityRegular"<<std::endl;
//    BOOST_FOREACH(ChaosSharedPtr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
//    
//    found_flag_for_severity.clear();
//    catalog_b.getFlagsForSeverity(StateFlagServerityWarning, found_flag_for_severity);
//    std::cout << "catalog_b StateFlagServerityWarning"<<std::endl;
//    BOOST_FOREACH(ChaosSharedPtr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
//    
//    found_flag_for_severity.clear();
//    catalog_b.getFlagsForSeverity(StateFlagServerityWarning, found_flag_for_severity);
//    std::cout << "catalog_b StateFlagServerityCritical"<<std::endl;
//    BOOST_FOREACH(ChaosSharedPtr<StateFlag> flag, found_flag_for_severity ){std::cout << flag->getName()  << std::endl;}
//    
//    //print serialization
//    StateFlagCatalogSDWrapper sfcsdw;
//    sfcsdw.dataWrapped() = catalog_b;
//    std::cout << sfcsdw.serialize()->getJSONString() << std::endl;
}

