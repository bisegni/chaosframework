//
//  main.cpp
//  PluginLoader
//
//  Created by Claudio Bisegni on 6/22/13.
//
//
#include <string>
#include <vector>
#include <chaos/cu_toolkit/driver_manager/driver/DriverPluginLoader.h>
#include <chaos/common/plugin/PluginLoader.h>

using namespace chaos::common::plugin;
using namespace chaos::cu::dm::driver;
int main(int argc, const char * argv[]) {
    AbstractPlugin *plugin = NULL;
    
   
        DriverPluginLoader driverLoader("libPluginDefinition.extension");
        if(driverLoader.loaded()) {
            //boost::function<AbstractDriverPlugin*()> instance = lib.get<AbstractDriverPlugin*>("DriverAlias_allocator");
            AbstractDriverPlugin * adp = driverLoader.newDriverInstance("DriverAlias");
            if(adp) {
                std::cout << adp->getName() << "-" << adp->getType() << "-" << adp->getVersion() << std::endl;
                const DriverAccessor *da = adp->getNewAccessor();
                if(da) {
                    adp->releaseAccessor((DriverAccessor *)da);
                }
                delete adp;
            }
        }
        
        PluginLoader loader("libPluginDefinition.extension");
        if(loader.loaded()) {
        PluginInspector *inspector = loader.getInspector();
        
        std::cout << "Registered plugin names: " << inspector->getNamesSize() << std::endl;
        for (int idx = 0; idx < inspector->getNamesSize() ; idx++) {
           
            std::string registeredName = inspector->getNameForIndex(idx);
            
            size_t numberOfAttributes = inspector->getInputAttributeByNamesSize(registeredName.c_str());
            
            for(int idx = 0; idx < numberOfAttributes; idx++) {
                std::cout << "Attribute: " << inspector->getInputAttributeForNameAndIndex(registeredName.c_str(), idx) << " for plugin: " << registeredName << std::endl;
            }
            
            std::cout << "Get instance for plugin: " << registeredName << std::endl;
            plugin = loader.newInstance(registeredName.c_str());
            if(plugin) {
                std::cout << plugin->getName() << "-" << plugin->getType() << "-" << plugin->getVersion() << std::endl;
                delete plugin;
            }
        }
    }
}
