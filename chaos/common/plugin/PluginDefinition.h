//
//  Definition.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 6/22/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_Definition_h
#define CHAOSFramework_Definition_h


#include <iostream>
#include <boost/extension/extension.hpp>
#include <chaos/common/plugin/AbstractPlugin.h>


namespace chaos {
    namespace common{
        namespace plugin {
            
#define ALLOCATOR(a,v,t,n) \
extern "C" \
void* BOOST_EXTENSION_EXPORT_DECL \
a ## _allocator() {\
PluginInstancer<n> instancer(#a,#v,#t);\
return instancer.getInstance();\
}
            
            
#define DEFINE_PLUGIN_CLASS(a, v, t, n)\
class n;\
ALLOCATOR(a,v,t,n) \
\
class n : public AbstractPlugin
            
        }
    }
}

#endif

