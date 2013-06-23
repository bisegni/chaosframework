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
    
    PluginAllocator allocator("PLUG_TEST_ALIAS","libPluginDefinition.extension");
    
    if(allocator.loaded()) {
        for (int idx = 0; idx < 100; idx++) {
            AbstractPlugin *pluginInstance = allocator.newInstance();
            allocator.releaseInstance(pluginInstance);
        }
        std::cout << "test\n";        
    }
}
