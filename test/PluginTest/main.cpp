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
using namespace chaos::cu::driver_manager::driver;
int main(int argc, const char * argv[]) {
    AbstractPlugin *plugin = NULL;
    std::cout << "----------------------------------Start general plugin test----------------------------------" << std::endl;
    PluginLoader loader("PluginTest.extension");
    if(loader.loaded()) {
        auto_ptr<PluginDiscover> discover(loader.getDiscover());
        
        std::cout << "Registered plugin names: " << discover->getNamesSize() << std::endl;
        for (int idx = 0; idx < discover->getNamesSize() ; idx++) {
            
            const char * registeredName = discover->getNameForIndex(idx);
            
            auto_ptr<PluginInspector> inspector(loader.getInspectorForName(registeredName));
            
            size_t numberOfAttributes = inspector->getInputAttributeByNamesSize(registeredName);
            
            for(int idx = 0; idx < numberOfAttributes; idx++) {
                std::cout << "Attribute: " << inspector->getInputAttributeForNameAndIndex(registeredName, idx) << " for plugin: " << registeredName << std::endl;
            }
            
            std::cout << "Get instance for plugin: " << registeredName << std::endl;
            plugin = loader.newInstance(registeredName);
            if(plugin) {
                std::cout << plugin->getName() << "-" << plugin->getType() << "-" << plugin->getVersion() << std::endl;
                delete plugin;
            } else {
                std::cout << registeredName << " not instantiated" << std::endl;
            }
        }
    }else {
        std::cerr << "Plugin not found" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "----------------------------------End general plugin test----------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------Start driver plugin test----------------------------------" << std::endl;
    DriverPluginLoader driverLoader("PluginTest.extension");
    if(driverLoader.loaded()) {
        auto_ptr<PluginInspector> inspector(driverLoader.getInspectorForName("DriverAlias"));
        
        size_t numberOfAttributes = inspector->getInputAttributeByNamesSize("DriverAlias");
        
        for(int idx = 0; idx < numberOfAttributes; idx++) {
            std::cout << "Attribute: " << inspector->getInputAttributeForNameAndIndex("DriverAlias", idx) << " for plugin: " << "DriverAlias" << std::endl;
        }
        
        //boost::function<AbstractDriverPlugin*()> instance = lib.get<AbstractDriverPlugin*>("DriverAlias_allocator");
        AbstractDriverPlugin * adp = driverLoader.newDriverInstance("DriverAlias");
        if(adp) {
            std::cout << adp->getName() << "-" << adp->getType() << "-" << adp->getVersion() << std::endl;
            const DriverAccessor *da = adp->getNewAccessor();
            if(da) {
                adp->releaseAccessor((DriverAccessor *)da);
            }
            delete adp;
        } else {
            std::cout << "DriverAlias not instantiated" << std::endl;
        }
    } else {
        std::cerr << "Plugin not found" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "----------------------------------End driver plugin test----------------------------------" << std::endl;

    return EXIT_SUCCESS;
}
