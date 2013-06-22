//
//  main.cpp
//  PluginLoader
//
//  Created by Claudio Bisegni on 6/22/13.
//
//

#include <chaos/common/plugin/PluginAllocator.h>

using namespace chaos::plugin;

int main(int argc, const char * argv[]) {
    
    PluginAllocator allocator("PLUG_TEST_ALIAS","/Coding/Sorgenti/gitrepos/chaos/DerivedData/CHAOSWorkspace/Build/Products/Debug/libPluginDefinition.dylib");
    
    if(allocator.loaded()) {
        AbstractPlugin *pluginInstance = allocator.newInstance();
        std::cout << "test\n";
    }
}
