/*
 *	EUPluginLoader.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/06/2017 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
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
                            EUPluginLoader(const char *pluginPath);
                            
                            //! default desctructor
                            ~EUPluginLoader();
                            
                            //! give infromation about the load operation of the dll
                            bool loaded();
                            
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
