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

using namespace chaos::plugin;


DEFINE_PLUGIN_CLASS(PLUG_TEST_ALIAS, PluginTest) {
public:
    const char *printTest();
};



#endif /* defined(__PluginTest__PluginTest__) */
