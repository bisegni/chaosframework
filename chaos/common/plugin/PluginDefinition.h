/*
 *	PluginDefinition.h
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

#ifndef CHAOSFramework_PluginDefinition_h
#define CHAOSFramework_PluginDefinition_h

#include <chaos/common/extension/extension.hpp>

#include <chaos/common/plugin/PluginDiscover.h>
#include <chaos/common/plugin/PluginInspector.h>
#include <chaos/common/plugin/AbstractPlugin.h>
namespace chaos {
    namespace common {
        
        //! Name space for grupping class for the plugin managment system of !CHAOS framework
        namespace plugin {
			
            //! Define a class as plugin
            /*!
             Every class that extend abstract plugin can be defined as plugin usign this macro.
             \param a alias of the plugin
             \param v the version of the plugin
             \param t the type of the plugin
             \param n the name of the class that implement the plugin
             */
#define DEFINE_PLUGIN_ALLOCATOR(a,v,t,n,r) \
extern "C" \
r* BOOST_EXTENSION_EXPORT_DECL \
a ## _allocator() {\
return chaos::common::plugin::PluginInstancer<n>::getInstance(#a,#v,#t);\
}

			//! retunr the plugin definition instance lcass for the driver alias in input
#define GET_PLUGIN_CLASS_DEFINITION(a) a ## _inspector()
	
			
#define DEFINE_PLUGIN_DEFINITION_PROTOTYPE(a,r) extern "C" chaos::common::plugin::PluginInspector* BOOST_EXTENSION_EXPORT_DECL a ## _inspector();
			
            //!
#define OPEN_GENERAL_PLUGIN_CLASS_DEFINITION(a,v,t,n,r) \
DEFINE_PLUGIN_ALLOCATOR(a,v,t,n, r) \
extern "C" \
chaos::common::plugin::PluginInspector* BOOST_EXTENSION_EXPORT_DECL \
a ## _inspector() {\
chaos::common::plugin::PluginInspector *inspector = new chaos::common::plugin::PluginInspector();\
inspector->setName(#a); \
inspector->setType(#t); \
inspector->setVersion(#v); \
inspector->setSubclass(#r);
            
            //!
#define OPEN_PLUGIN_CLASS_DEFINITION(a,v,t,n) \
OPEN_GENERAL_PLUGIN_CLASS_DEFINITION(a,v,t,n, chaos::common::plugin::AbstractPlugin)


            //! Register an init paramter
            /*!
             Register an input attribute for a pluginusing the plaugin alias
             \param a plugin alias for wich we need to register the attribute
             \param ia the input attribute name
             */
#define REGISTER_PLUGIN_CLASS_INIT_ATTRIBUTE(a, ia)\
inspector->addInitAttributeForName(#a, #ia);

            //! Close and end the plugin registrations phase
#define CLOSE_PLUGIN_CLASS_DEFINITION \
return inspector; \
}

            //! Start the registration of the plugin
            /*!
             The registration phase of the plugins permit to expose the aslias and the init attribute for all registered plugin
             using the PLuginLoader class
             */

#ifdef CHAOS_ENABLE_PLUGIN
#define OPEN_REGISTER_PLUGIN \
extern "C" \
chaos::common::plugin::PluginDiscover* BOOST_EXTENSION_EXPORT_DECL \
getDiscover() {\
chaos::common::plugin::PluginDiscover *discover = new chaos::common::plugin::PluginDiscover();

            //! Register a plugin
            /*!
             Register an alias of a plugin
             \param a plugin alias to register
             */
#define REGISTER_PLUGIN(a)\
discover->addName(#a);

            //!
#define CLOSE_REGISTER_PLUGIN \
return discover;\
 }
#else
#define OPEN_REGISTER_PLUGIN
#define CLOSE_REGISTER_PLUGIN
#define REGISTER_PLUGIN(a)
#endif

        }
    }
}
#endif
