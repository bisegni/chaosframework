/*
 *	trigger_system_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h
#define __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataVariant.h>

#include <chaos/common/property/PropertyGroup.h>
#include <chaos/common/utility/ObjectInstancer.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::data::CDataVariant, CDataVariantVector)
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, chaos::common::data::CDataVariant, MapKeyCDataVariant)
                
                typedef enum ConsumerResult {
                    //!value has been accepted
                    ConsumerResultOK,
                    ConsumerResultWarinig,
                    ConsumerResultCritical
                } ConsumerResult;
                
                template<typename B>
                class ConsumerInstanceDescription:
                public chaos::common::property::PorpertyGroup {
                    std::auto_ptr< chaos::common::utility::ObjectInstancer<B> > consumer_instancer;
                protected:
                    ConsumerInstanceDescription(const std::string& name,
                                                std::auto_ptr< chaos::common::utility::ObjectInstancer<B> > auto_instancer):
                    PorpertyGroup(name),
                    consumer_instancer(auto_instancer){}
                    
                public:
                    std::auto_ptr<B> getAutoInstance(){
                        return std::auto_ptr<B>(consumer_instancer->getInstance());
                    }
                    
                    B* getInstance(){
                        return consumer_instancer->getInstance();
                    }
                };
                
                //!consumer description
                /*!
                 Permit to collect the list of porperty taht permit to configure
                 the execution of the consumer
                 */
                template<typename I, typename B>
                class ConsumerPropertyDescription:
                public ConsumerInstanceDescription<B> {
                public:
                    ConsumerPropertyDescription(const std::string& name):
                    ConsumerInstanceDescription<B>(name,
                                                   std::auto_ptr< chaos::common::utility::ObjectInstancer<B> >(new chaos::common::utility::TypedObjectInstancer<I, B>())) {}
                    
                };
                
                //!helper macro
#define CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(impl, base)\
class impl;\
class impl ## PropertyDescription:\
public chaos::cu::data_manager::trigger_system::ConsumerPropertyDescription<impl, base> {\
public: impl ## PropertyDescription():\
ConsumerPropertyDescription<impl, base>(#impl) {
                
                
#define CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY(name, desc, type)\
addProperty(name, desc, type);
                
#define CHAOS_TRIGGER_CONSUMER_CLOSE_DESCRIPTION()\
}\
};
                
                
#define CHAOS_TRIGGER_CONSUMER_ADD_DEFINITION(impl, base, desc)\
CHAOS_TRIGGER_CONSUMER_CLOSE_DESCRIPTION()\
class impl:\
public base {\
public:\
impl():base(#impl, desc){};\
~impl(){};\

#define CHAOS_TRIGGER_CONSUMER_CLOSE_DEFINITION()\
};

            }
        }
    }
}

#endif /* __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h */
