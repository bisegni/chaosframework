//
//  PluginTest.cpp
//  PluginTest
//
//  Created by Claudio Bisegni on 6/22/13.
//
//

#include "PluginTest.h"

//define the general plugin

OPEN_PLUGIN_CLASS_DEFINITION(PluginAlias, 1.0, DRV, PluginTest)
CLOSE_PLUGIN_CLASS_DEFINITION

//define the cu driver plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(DriverAlias, 1.0, Sl7TcpDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(DriverAlias,http_address)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(DriverAlias,http_port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//register the two plugin
OPEN_REGISTER_PLUGIN
    REGISTER_PLUGIN(PluginAlias)
    REGISTER_PLUGIN(DriverAlias)
CLOSE_REGISTER_PLUGIN

PluginTest::PluginTest() {
     std::cout << "PluginTest created" << std::endl;
}

void PluginTest::test(int num) {
    for(int idx = 0; idx < num; idx++) {
        std::cout << "Plugin in test output" << std::endl;
    }
}


DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(Sl7TcpDriver) {
    std::cout << "Driver plugin created" << std::endl;
}