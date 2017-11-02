/*
 * Copyright 2012, 02/11/2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include <gtest/gtest.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverPluginLoader.h>
#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUPluginLoader.h>
#include <chaos/common/plugin/PluginLoader.h>
#include <chaos/common/plugin/PluginManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
using namespace chaos::common::plugin;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::cu::control_manager::script::api::plugin;


//TEST(TestPlugin, General) {
//    ChaosUniquePtr<AbstractPlugin> plugin;
//    PluginLoader loader("PluginLibrary.chaos_extension");
//    ASSERT_TRUE(loader.loaded());
//    
//    ChaosUniquePtr<PluginDiscover> discover(loader.getDiscover());
//    
//    ASSERT_GT(discover->getNamesSize(), 0);
//    for (int idx = 0; idx < discover->getNamesSize() ; idx++) {
//        
//        const char * registeredName = discover->getNameForIndex(idx);
//
//        ChaosUniquePtr<PluginInspector> inspector(loader.getInspectorForName(registeredName));
//        ASSERT_TRUE(inspector.get());
//        size_t numberOfAttributes = inspector->getInputAttributeByNamesSize(registeredName);
//        std::string subclass = inspector->getSubclass();
//        if(subclass.compare("chaos::common::plugin::AbstractPlugin") == 0) {
//            plugin = loader.newInstance<AbstractPlugin>(registeredName);
//            ASSERT_TRUE(plugin.get());
//        }
//    }
//}
//int main(int argc, char ** argv) {
//    chaos::GlobalConfiguration::getInstance()->preParseStartupParameters();
//    chaos::GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
//    ChaosUniquePtr<AbstractPlugin> plugin;
//    std::cout << "----------------------------------Start general plugin test----------------------------------" << std::endl;
//    PluginLoader loader("PluginLibrary.chaos_extension");
//    if(loader.loaded()) {
//        ChaosUniquePtr<PluginDiscover> discover(loader.getDiscover());
//
//        std::cout << "Registered plugin names: " << discover->getNamesSize() << std::endl;
//        for (int idx = 0; idx < discover->getNamesSize() ; idx++) {
//
//            const char * registeredName = discover->getNameForIndex(idx);
//
//            std::cout << "Found plugin: " << registeredName << std::endl;
//            ChaosUniquePtr<PluginInspector> inspector(loader.getInspectorForName(registeredName));
//
//            size_t numberOfAttributes = inspector->getInputAttributeByNamesSize(registeredName);
//            if(numberOfAttributes) {
//                std::cout << "Plugin has " << numberOfAttributes <<  " attribute" << std::endl;
//                for(int idx = 0; idx < numberOfAttributes; idx++) {
//                    std::cout << "Attribute: " << inspector->getInputAttributeForNameAndIndex(registeredName, idx) << " for plugin: " << registeredName << std::endl;
//                }
//            }
//            std::string subclass = inspector->getSubclass();
//            if(subclass.compare("chaos::common::plugin::AbstractPlugin") == 0) {
//                plugin = loader.newInstance<AbstractPlugin>(registeredName);
//                if(plugin.get()) {
//                    std::cout << "Create instance for:" << plugin->getName() << "-" << plugin->getType() << "-" << plugin->getVersion() << std::endl;
//                }
//            }
//        }
//    }else {
//        std::cerr << "Plugin not found" << std::endl;
//        return EXIT_FAILURE;
//    }
//    std::cout << "----------------------------------End general plugin test-----------------------------------" << std::endl;
//    std::cout << std::endl;
//    std::cout << "----------------------------------Start driver plugin test----------------------------------" << std::endl;
//    char *out_mem = NULL;
//    uint32_t out_size = 0;
//    chaos::common::utility::InizializableService::initImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), NULL, "AsyncCentralManager", __PRETTY_FUNCTION__);
//    PluginManager pl_man("/Users/bisegni/source/chaos_development/chaosframework/chaos-distrib-x86_64-Darwin/bin");
//    pl_man.init(NULL);
//    ChaosUniquePtr<EUAbstractApiPlugin> pi = pl_man.getPluginInstanceBySubclassAndName<EUAbstractApiPlugin>("chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin", "EUPluginAlgotest");
//    pi->execute("test_str", strlen("test_str"), &out_mem, &out_size);
//    std::cout << out_mem << std::endl;
//    std::cout << pi->getApiName() << std::endl;
//    free(out_mem);
//    pl_man.deinit();
//    chaos::common::utility::InizializableService::deinitImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), "AsyncCentralManager", __PRETTY_FUNCTION__);
//    std::cout << "----------------------------------End driver plugin test----------------------------------" << std::endl;
//
//    return EXIT_SUCCESS;
//}

