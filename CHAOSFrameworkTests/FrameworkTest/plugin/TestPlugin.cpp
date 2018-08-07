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


TEST(PluginTest, General) {
    PluginLoader loader("ExamplePlugin.chaos_extension");
    ASSERT_TRUE(loader.loaded());
    
    ChaosUniquePtr<PluginDiscover> discover(loader.getDiscover());
    
    ASSERT_GT(discover->getNamesSize(), 0);
    for (int idx = 0; idx < discover->getNamesSize() ; idx++) {
        
        const char * registeredName = discover->getNameForIndex(idx);
        
        ChaosUniquePtr<PluginInspector> inspector(loader.getInspectorForName(registeredName));
        ASSERT_TRUE(inspector.get());
        ASSERT_EQ(inspector->getInputAttributeByNamesSize(registeredName), 0);
        std::string subclass = inspector->getSubclass();
        if(subclass.compare("chaos::common::plugin::AbstractPlugin") == 0) {
            ChaosUniquePtr<AbstractPlugin> plugin;
            plugin = loader.newInstance<AbstractPlugin>(registeredName);
            ASSERT_TRUE(plugin.get());
            ASSERT_EQ(plugin->init(), 0);
            ASSERT_NO_THROW(plugin->deinit());
        } else if(subclass.compare("chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin") == 0) {
            ChaosUniquePtr<chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin> plugin;
            std::string test_in = "test in data";
            char * test_out = NULL;
            uint32_t test_out_size = 0;
            plugin = loader.newInstance<chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin>(registeredName);
            ASSERT_TRUE(plugin.get());
            ASSERT_EQ(plugin->init(), 0);
            ASSERT_EQ(plugin->execute(test_in.c_str(),
                                      test_in.size(),
                                      &test_out,
                                      &test_out_size),
                      0);
            ASSERT_TRUE(test_out);
            ASSERT_EQ(test_in.size()+5, test_out_size);
            ASSERT_NO_THROW(plugin->deinit());
            delete(test_out);
        }
    }
}

