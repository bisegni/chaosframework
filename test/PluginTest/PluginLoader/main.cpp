//
//  main.cpp
//  PluginLoader
//
//  Created by Claudio Bisegni on 6/22/13.
//
//

#include <chaos/common/plugin/PluginAllocator.h>
#define PLUGIN_NUMBERS 1000
using namespace chaos::common::plugin;

int main(int argc, const char * argv[]) {
    AbstractPlugin *plugin[PLUGIN_NUMBERS];
    PluginAllocator allocator("PLUGIN_ALIAS","libPluginDefinition.extension");
    
    if(allocator.loaded()) {
        for (int idx = 0; idx < PLUGIN_NUMBERS; idx++) {
            plugin[idx] = allocator.newInstance();
        }
     
        for (int idx = 0; idx < PLUGIN_NUMBERS; idx++) {
            std::cout << plugin[idx]->getName() << "-" << plugin[idx]->getVersion() << "-"<< plugin[idx]->getType() <<"\n";// <<  ptest->printTest() << endl;
            delete plugin[idx];
        }
    }
}
