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
    namespace common {
        namespace trigger {
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
                public chaos::common::property::PropertyGroup {
                    std::auto_ptr< chaos::common::utility::ObjectInstancer<B> > consumer_instancer;
                protected:
                    ConsumerInstanceDescription(const std::string& name,
                                                std::auto_ptr< chaos::common::utility::ObjectInstancer<B> > auto_instancer):
                    PropertyGroup(name),
                    consumer_instancer(auto_instancer){}
                    
                public:
                    std::auto_ptr<B> getAutoInstance(){
                        return std::auto_ptr<B>(getInstance());
                    }
                    
                    B* getInstance(){
                        B* new_instance = consumer_instancer->getInstance();
                        //copy property from twhi group
                        chaos::common::property::PropertyGroup *pg_instance = dynamic_cast<chaos::common::property::PropertyGroup*>(new_instance);
                        if(pg_instance) {
                            new_instance->copyPropertiesFromGroup(*this);
                        }
                        return new_instance;
                    }
                    
                    const std::string& getInstanceDescriptionName() const {
                        return getGroupName();
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
        }
    }
}

#endif /* __CHAOSFramework_F51EAA5C_833E_40F9_A315_AB754895AC25_trigger_system_types_h */
