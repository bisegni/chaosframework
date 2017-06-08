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
#include <chaos/common/plugin/PluginManager.h>
using namespace chaos::common::plugin;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::cu::control_manager::script::api::plugin;

int main(int argc, const char * argv[]) {
    ChaosUniquePtr<AbstractPlugin> plugin;
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
            std::string subclass = inspector->getSubclass();
            if(subclass.compare("chaos::common::plugin::AbstractPlugin") == 0) {
                plugin = loader.newInstance<AbstractPlugin>(registeredName);
                if(plugin.get()) {
                    std::cout << "Create instance for:" << plugin->getName() << "-" << plugin->getType() << "-" << plugin->getVersion() << std::endl;
                }
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
    char *out_mem = NULL;
    uint32_t out_size = 0;
    chaos::common::utility::InizializableService::initImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), NULL, "AsyncCentralManager", __PRETTY_FUNCTION__);
    PluginManager pl_man("/Users/bisegni/source/chaos_development/chaosframework/chaos-distrib-x86_64-Darwin/bin");
    pl_man.init(NULL);
    ChaosUniquePtr<EUAbstractApiPlugin> pi = pl_man.getPluginInstanceBySubclassAndName<EUAbstractApiPlugin>("chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin", "EUPluginAlgotest");
    pi->execute("test_str", strlen("test_str"), &out_mem, &out_size);
    std::cout << out_mem << std::endl;
    std::cout << pi->getApiName() << std::endl;
    free(out_mem);
    pl_man.deinit();
    chaos::common::utility::InizializableService::deinitImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), "AsyncCentralManager", __PRETTY_FUNCTION__);
    std::cout << "----------------------------------End driver plugin test----------------------------------" << std::endl;
    
    return EXIT_SUCCESS;
}
