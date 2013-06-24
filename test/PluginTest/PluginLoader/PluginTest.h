//
//  PluginTest.h
//  PluginTest
//
//  Created by Claudio Bisegni on 6/22/13.
//
//

#ifndef __PluginTest__PluginTest__
#define __PluginTest__PluginTest__

#include <chaos/common/plugin/PluginDefinition.h>

#include <chaos/cu_toolkit/driver_manager/driver/DriverWrapperPlugin.h>

using namespace chaos::common::plugin;
using namespace chaos::cu::dm::driver;

DEFINE_PLUGIN_CLASS(PLUGIN_ALIAS, 1.0, DRV, PluginTest) {
public:
    virtual const char *printTest();
};
/*
REGISTER_CU_DRIVER_CLASS(DRIVER_ALIAS, 1.0, DRV, Sl7TcpDriver)
public:
    //! Execute a command
    void execOpcode(DrvMsgPtr cmd) {
        
    }
};*/

#endif /* defined(__PluginTest__PluginTest__) */
