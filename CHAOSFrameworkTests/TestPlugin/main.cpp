//
//  main.cpp
//  TestPlugin
//
//  Created by bisegni on 23/08/16.
//  Copyright © 2016 bisegni. All rights reserved.
//
#include <string>
#include <vector>
#include <chaos/cu_toolkit/driver_manager/driver/DriverPluginLoader.h>
#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUPluginLoader.h>
#include <chaos/common/plugin/PluginLoader.h>

using namespace chaos::common::plugin;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::cu::control_manager::script::api::plugin;

int main(int argc, const char * argv[]) {
    AbstractPlugin *plugin = NULL;
    std::cout << "----------------------------------Start general plugin test----------------------------------" << std::endl;
    PluginLoader loader("PluginLibrary.chaos_extension");
    if(loader.loaded()) {
        ChaosUniquePtr<PluginDiscover> discover(loader.getDiscover());
        
        std::cout << "Registered plugin names: " << discover->getNamesSize() << std::endl;
        for (int idx = 0; idx < discover->getNamesSize() ; idx++) {
            
            const char * registeredName = discover->getNameForIndex(idx);
            
            std::cout << "Found plugin: " << registeredName << std::endl;
            ChaosUniquePtr<PluginInspector> inspector(loader.getInspectorForName(registeredName));
            
            size_t numberOfAttributes = inspector->getInputAttributeByNamesSize(registeredName);
            if(numberOfAttributes) {
                std::cout << "Plugin has " << numberOfAttributes <<  " attribute" << std::endl;
                for(int idx = 0; idx < numberOfAttributes; idx++) {
                    std::cout << "Attribute: " << inspector->getInputAttributeForNameAndIndex(registeredName, idx) << " for plugin: " << registeredName << std::endl;
                }
            }
            plugin = loader.newInstance(registeredName);
            if(plugin) {
                std::cout << "Create instance for:" << plugin->getName() << "-" << plugin->getType() << "-" << plugin->getVersion() << std::endl;
                delete plugin;
            } else {
                std::cerr << registeredName << " not instantiated" << std::endl;
            }
        }
    }else {
        std::cerr << "Plugin not found" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "----------------------------------End general plugin test-----------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------Start driver plugin test----------------------------------" << std::endl;
    DriverPluginLoader driverLoader("PluginLibrary.chaos_extension");
    if(driverLoader.loaded()) {
        ChaosUniquePtr<PluginInspector> inspector(driverLoader.getInspectorForName("DriverAlias"));
        
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
    std::cout << std::endl;
    std::cout << "----------------------------------Start driver plugin test----------------------------------" << std::endl;
    EUPluginLoader euPluginLoader("PluginLibrary.chaos_extension");
    if(driverLoader.loaded()) {
        ChaosUniquePtr<PluginInspector> eu_plugin_inspector(euPluginLoader.getInspectorForName("EUPluginAlgotest"));
        ChaosUniquePtr<EUAbstractApiPlugin> eu_plugin_instance(euPluginLoader.newPluginInstance("EUPluginAlgotest"));
        if(eu_plugin_instance.get()) {
            std::cout << eu_plugin_instance->getName() << "-" << eu_plugin_instance->getType() << "-" << eu_plugin_instance->getVersion() << std::endl;
            std::string test_input("Eu script plugin input data");
            char *out_data = NULL;
            uint32_t out_size;
            eu_plugin_instance->execute(test_input.c_str(),
                                        (uint32_t)test_input.size(),
                                        &out_data,
                                        &out_size);
            if(out_data) {
                 std::cout << out_data << std::endl;
            }
            free(out_data);
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
