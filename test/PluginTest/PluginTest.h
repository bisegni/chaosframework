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

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

using namespace chaos::common::plugin;

class PluginTest: public AbstractPlugin {
public:
    PluginTest();
    void test(int num);
};

using namespace chaos::cu::driver_manager::driver;
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(Sl7TcpDriver)
class Sl7TcpDriver: ADD_CU_DRIVER_PLUGIN_SUPERCLASS {
    
public:
    Sl7TcpDriver();
    //! Execute a command
    MsgManagmentResultType::MsgManagmentResult execOpcode(DrvMsgPtr cmd) {
        return MsgManagmentResultType::MMR_EXECUTED;
    }
};

#endif /* defined(__PluginTest__PluginTest__) */
