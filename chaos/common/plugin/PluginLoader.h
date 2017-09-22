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

#ifndef CHAOSFramework_PluginLoader_h
#define CHAOSFramework_PluginLoader_h

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <boost/function.hpp>
#include <chaos/common/chaos_types.h>
#include <chaos/common/extension/shared_library.hpp>
#include <chaos/common/plugin/PluginInspector.h>
#include <chaos/common/plugin/PluginDiscover.h>
#include <chaos/common/plugin/AbstractPlugin.h>

namespace chaos {
    namespace common{
        namespace plugin {
            
            
            
#define SYM_ALLOC_POSTFIX "_allocator"
            
            //! Plugin class loader and allocator
            /*!
             Plugins Allcoator class try to find th elibrary and permit to allocate instance
             of plugin class.
             */
            class PluginLoader {
                //Allocator plugin exported function
                boost::function<PluginDiscover*()> getDiscoverFunction;
            protected:
                boost::extensions::shared_library lib;
                
                bool checkPluginInstantiableForSubclass(const std::string& pluginName,
                                                        const std::string& subclass);
            public:
                PluginLoader(const std::string& plugin_path);
                
                ~PluginLoader();
                
                bool loaded();
                
                PluginDiscover* getDiscover();
                
                PluginInspector* getInspectorForName(const std::string& plugin_name);
                
                template<typename C>
                ChaosUniquePtr<C> newInstance(const std::string& plugin_name) {
                    
                    //check if lib is loaded
                    if(!loaded()) return ChaosUniquePtr<C>();
                    
                    //we can instantiate the plugin
                    std::string allocator_name = plugin_name + SYM_ALLOC_POSTFIX;
                    
                    //try to get function allocator
                    boost::function<C*()>  instancer = lib.get<C*>(allocator_name);
                    return (instancer != NULL) ? ChaosUniquePtr<C>(instancer()):ChaosUniquePtr<C>();
                }
            };
        }
    }
}
#endif
