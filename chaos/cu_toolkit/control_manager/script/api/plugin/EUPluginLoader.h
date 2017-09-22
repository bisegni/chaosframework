/*
 * Copyright 2012, 2017 INFN
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

#ifndef __CHAOSFramework__FCE7A45_F496_4D55_AACB_612532713721_EUPluginLoader_h
#define __CHAOSFramework__FCE7A45_F496_4D55_AACB_612532713721_EUPluginLoader_h
#include <chaos/common/plugin/PluginLoader.h>
#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUAbstractApiPlugin.h>
namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace script {
                namespace api {
                    namespace plugin {
                        //! Execution unit plugin loader
                        /*!
                         PluginLoader subclass for the loading of the control unit driver plugin
                         */
                        class EUPluginLoader:
                        private chaos::common::plugin::PluginLoader {
                            std::map<std::string, boost::function<EUAbstractApiPlugin*()> > driverAllocatorFunctions;
                        public:
                            //! default constructor
                            EUPluginLoader(const std::string& plugin_path);
                            
                            //! default desctructor
                            ~EUPluginLoader();
                            
                            //! give infromation about the load operation of the dll
                            bool loaded();
                            
                            ChaosUniquePtr<common::plugin::PluginDiscover> getDiscover();
                            
                            //! Get the inspector for the name
                            ChaosUniquePtr<chaos::common::plugin::PluginInspector> getInspectorForName(const std::string& plugin_name);
                            
                            //! Return the instance of the driver
                            ChaosUniquePtr<EUAbstractApiPlugin> newPluginInstance(const std::string& pluginName);
                        };
                    }
                }
            }
        }
    }
}
#endif /* __CHAOSFramework__FCE7A45_F496_4D55_AACB_612532713721_EUPluginLoader_h */
