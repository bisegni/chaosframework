/*
 *	CDataWrapperKeyValueSetter.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef CHAOSFramework_CDataWrapperSetter_h
#define CHAOSFramework_CDataWrapperSetter_h

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace common {
        namespace data {
            
            class CDataWrapperKeyValueSetter {
            protected:
                const std::string attribute_name;
                
                CDataWrapperKeyValueSetter(const std::string& _attribute_name):
                attribute_name(_attribute_name){}
            public:
                virtual void setTo(chaos::common::data::CDataWrapper& data) = 0;
            };
            
            template<typename type>
            class TypedCDataWrapperKeyValueSetter:
            public CDataWrapperKeyValueSetter {
            protected:
                type attribute_value;
                TypedCDataWrapperKeyValueSetter(const std::string _attribute_name,
                                                const type& _attribute_value):
                CDataWrapperKeyValueSetter(_attribute_name),
                attribute_value(_attribute_value){}
            };
            
            class CDataWrapperBoolKeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<bool> {
            public:
                CDataWrapperBoolKeyValueSetter(const std::string _attribute_name,
                                               const bool& _attribute_value):
                TypedCDataWrapperKeyValueSetter<bool>(attribute_name,
                                                      attribute_value){}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addBoolValue(attribute_name,
                                      attribute_value);
                }
            };
            
            class CDataWrapperInt32KeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<int32_t> {
                public:
                CDataWrapperInt32KeyValueSetter(const std::string _attribute_name,
                                                const int32_t& _attribute_value):
                TypedCDataWrapperKeyValueSetter<int32_t>(attribute_name,
                                                         attribute_value){}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addInt32Value(attribute_name,
                                       attribute_value);
                }
            };
            
            class CDataWrapperInt64KeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<int64_t> {
                public:
                CDataWrapperInt64KeyValueSetter(const std::string _attribute_name,
                                                const int64_t& _attribute_value):
                TypedCDataWrapperKeyValueSetter<int64_t>(attribute_name,
                                                         attribute_value){}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addInt64Value(attribute_name,
                                       attribute_value);
                }
            };
            
            class CDataWrapperDoubleKeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<double> {
                public:
                CDataWrapperDoubleKeyValueSetter(const std::string _attribute_name,
                                                 const double& _attribute_value):
                TypedCDataWrapperKeyValueSetter<double>(attribute_name,
                                                        attribute_value){}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addDoubleValue(attribute_name,
                                        attribute_value);
                }
            };
            
            class CDataWrapperStringKeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<string> {
                public:
                CDataWrapperStringKeyValueSetter(const std::string _attribute_name,
                                                 const string& _attribute_value):
                TypedCDataWrapperKeyValueSetter<string>(attribute_name,
                                                         attribute_value){}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addStringValue(attribute_name,
                                        attribute_value);
                }
            };
        }
    }
}
#endif
