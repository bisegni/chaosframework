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
            
            template<typename t>
            class TypedCDataWrapperKeyValueSetter:
            public CDataWrapperKeyValueSetter {
            protected:
                t attribute_value;
                TypedCDataWrapperKeyValueSetter(const std::string& _attribute_name):
                CDataWrapperKeyValueSetter(_attribute_name){}
            public:
                virtual void setValue(const t& new_value) {
                    attribute_value = new_value;
                }
                virtual const t& getValue() {
                    return attribute_value;
                }
            };
            
            class CDataWrapperNullKeyValueSetter:
            public CDataWrapperKeyValueSetter {
            public:
                CDataWrapperNullKeyValueSetter(const std::string& _attribute_name):
                CDataWrapperKeyValueSetter(_attribute_name){}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addNullValue(attribute_name);
                }
            };
            
            class CDataWrapperBoolKeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<bool> {
            public:
                CDataWrapperBoolKeyValueSetter(const std::string& _attribute_name,
                                               bool _attribute_value):
                TypedCDataWrapperKeyValueSetter<bool>(_attribute_name) {
                    attribute_value =_attribute_value;
                }
                
                CDataWrapperBoolKeyValueSetter(const std::string& _attribute_name):
                TypedCDataWrapperKeyValueSetter<bool>(_attribute_name)
                {attribute_value = false;}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addBoolValue(attribute_name,
                                      attribute_value);
                }
            };
            
            class CDataWrapperInt32KeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<int32_t> {
            public:
                CDataWrapperInt32KeyValueSetter(const std::string& _attribute_name,
                                                int32_t _attribute_value):
                TypedCDataWrapperKeyValueSetter<int32_t>(_attribute_name) {
                    attribute_value =_attribute_value;
                }
                CDataWrapperInt32KeyValueSetter(const std::string& _attribute_name):
                TypedCDataWrapperKeyValueSetter<int32_t>(_attribute_name)
                {attribute_value = 0;}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addInt32Value(attribute_name,
                                       attribute_value);
                }
            };
            
            class CDataWrapperInt64KeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<int64_t> {
            public:
                CDataWrapperInt64KeyValueSetter(const std::string& _attribute_name,
                                                int64_t _attribute_value):
                TypedCDataWrapperKeyValueSetter<int64_t>(_attribute_name) {
                    attribute_value =_attribute_value;
                }
                CDataWrapperInt64KeyValueSetter(const std::string& _attribute_name):
                TypedCDataWrapperKeyValueSetter<int64_t>(_attribute_name)
                {attribute_value = 0;}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addInt64Value(attribute_name,
                                       attribute_value);
                }
            };
            
            class CDataWrapperDoubleKeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<double> {
            public:
                CDataWrapperDoubleKeyValueSetter(const std::string& _attribute_name,
                                                 double _attribute_value):
                TypedCDataWrapperKeyValueSetter<double>(_attribute_name) {
                    attribute_value =_attribute_value;
                }
                CDataWrapperDoubleKeyValueSetter(const std::string& _attribute_name):
                TypedCDataWrapperKeyValueSetter<double>(_attribute_name)
                {attribute_value = 0;}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addDoubleValue(attribute_name,
                                        attribute_value);
                }
            };
            
            class CDataWrapperStringKeyValueSetter:
            public TypedCDataWrapperKeyValueSetter<std::string> {
            public:
                CDataWrapperStringKeyValueSetter(const std::string& _attribute_name,
                                                 const std::string& _attribute_value):
                TypedCDataWrapperKeyValueSetter<std::string>(_attribute_name) {
                    attribute_value =_attribute_value;
                }
                
                CDataWrapperStringKeyValueSetter(const std::string& _attribute_name):
                TypedCDataWrapperKeyValueSetter<std::string>(_attribute_name)
                {attribute_value = "";}
                
                void setTo(chaos::common::data::CDataWrapper& data) {
                    data.addStringValue(attribute_name,
                                        attribute_value);
                }
            };
            class CDataWrapperClusterKeyValueSetter:
                        public TypedCDataWrapperKeyValueSetter<std::string> {
                        public:
                            CDataWrapperClusterKeyValueSetter(const std::string& _attribute_name,
                                                             const std::string& _attribute_value):
                            TypedCDataWrapperKeyValueSetter<std::string>(_attribute_name) {
                                attribute_value =_attribute_value;
                            }

                            CDataWrapperClusterKeyValueSetter(const std::string& _attribute_name):
                            TypedCDataWrapperKeyValueSetter<std::string>(_attribute_name)
                            {attribute_value = "";}

                            void setTo(chaos::common::data::CDataWrapper& data) {
                                data.addJsonValue(attribute_name,
                                                    attribute_value);
                            }
                        };
            
            
        }
    }
}
#endif
