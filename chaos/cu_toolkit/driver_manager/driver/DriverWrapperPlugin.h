/*
 *	DriverWrapperPlugin.h
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

#ifndef CHAOSFramework_DriverWrapperPlugin_h
#define CHAOSFramework_DriverWrapperPlugin_h

#include <chaos/common/plugin/AbstractPlugin.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriver.h>

namespace chaos{
    namespace cu {
        namespace dm {
            namespace driver {
                    //! Plugin abstraction for the control unit driver
                /*!
                 
                 */
                template<typename T>
                class DriverWrapperPlugin : public chaos::common::plugin::AbstractPlugin {
                    AbstractDriver *driverReference;
                
                public:
                    DriverWrapperPlugin(){
                        driverReference = new T();
                    }
                    
                    ~ DriverWrapperPlugin() {
                        if(driverReference)
                            delete(driverReference);
                    }
                    
                    //! Proxy for create a new accessor to the driver
                    /*!
                     A new accessor is allocate. In the allocation process
                     the message queue for comunicating with this driver is
                     allocated.
                     */
                    bool getNewAccessor(DriverAccessor **newAccessor) {
                        return driverReference->getNewAccessor(newAccessor);
                    }
                    
                    //! Proxy for dispose an accessor
                    /*!
                     A driver accessor is relased and all resource are free.
                     */
                    void releaseAccessor(DriverAccessor *accessor) {
                        return driverReference->releaseAccessor(accessor);
                    }
                };
                
#define DRIVER_ALLOCATOR(a, v, t, n) \
class n; \
extern "C" \
void* BOOST_EXTENSION_EXPORT_DECL \
a ## _allocator() {\
PluginInstancer< DriverWrapperPlugin< n > > instancer(#a,#v,#t);\
return instancer.getInstance();\
}
                
#define REGISTER_CU_DRIVER_CLASS(a, v, t, n)\
DRIVER_ALLOCATOR(a, v, t, n) \
class n : public AbstractDriver {\
            template<typename T>\
            friend class DriverWrapperPlugin;\
            protected:\
                Sl7TcpDriver(){}\
                ~Sl7TcpDriver(){}\
                
            }
        }
    }
}

#endif
