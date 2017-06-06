/*
 *	PluginLoader.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#ifndef CHAOSFramework_PluginLoader_h
#define CHAOSFramework_PluginLoader_h

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <boost/function.hpp>
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
                
                PluginInspector* getInspectorForName(const std::string& pluginName);
                
                AbstractPlugin* newInstance(const std::string& pluginName);
            };
            
        }
    }
}
#endif
