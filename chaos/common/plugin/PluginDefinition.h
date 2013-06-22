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
    namespace plugin {
        
       
        
#define DEFINE_PLUGIN_CLASS(a, n)\
class n;\
PluginInstancer<n>* BOOST_EXTENSION_EXPORT_DECL \
a##Allocator() {\
return new PluginInstancer<n>();\
}\
class PluginTest : public AbstractPlugin\

    }
}

#endif

