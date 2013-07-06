/*
 *	PluginDefinition.h
 *	!CHOAS
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
        
        //! Name space for grupping class for the plugin managment system of !CHOAS framework
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
return PluginInstancer<n>::getInstance(#a,#v,#t);\
}
            
            //!
#define OPEN_GENERAL_PLUGIN_CLASS_DEFINITION(a,v,t,n,r) \
DEFINE_PLUGIN_ALLOCATOR(a,v,t,n, r) \
extern "C" \
PluginInspector* BOOST_EXTENSION_EXPORT_DECL \
a ## _inspector() {\
PluginInspector *inspector = new PluginInspector();\
inspector->setName(#a); \
inspector->setType(#t); \
inspector->setVersion(#v); \
inspector->setSubclass(#r);
            
            //!
#define OPEN_PLUGIN_CLASS_DEFINITION(a,v,t,n) \
OPEN_GENERAL_PLUGIN_CLASS_DEFINITION(a,v,t,n, AbstractPlugin)


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
#define OPEN_REGISTER_PLUGIN \
extern "C" \
PluginDiscover* BOOST_EXTENSION_EXPORT_DECL \
getDiscover() {\
PluginDiscover *discover = new PluginDiscover();

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
        }
    }
}
#endif
