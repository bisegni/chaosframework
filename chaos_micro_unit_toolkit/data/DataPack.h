/*
 *	DataPack.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/08/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__FF18E47_E86B_4A8B_B5CA_7EA5C789F2FB_DataPack_h
#define __CHAOSFramework__FF18E47_E86B_4A8B_B5CA_7EA5C789F2FB_DataPack_h

#include <chaos_micro_unit_toolkit/micro_unit_toolkit_types.h>
#include <chaos_micro_unit_toolkit/external_lib/json.h>

#include <vector>

namespace chaos {
    namespace micro_unit_toolkit {
        namespace data {
            class DataPack;
            typedef ChaosUniquePtr<DataPack> DataPackUniquePtr;
            typedef ChaosSharedPtr<DataPack> DataPackSharedPtr;
            //! main singleton lab entrypoint
            class DataPack {
                //data pointer is not owned by this object
                Json::Value root_json_object;
                DataPack(const Json::Value& src_root_json_object);
            public:
                DataPack();
                virtual ~DataPack();
                static DataPackUniquePtr newFromBuffer(const char *data,
                                                       const size_t data_len,
                                                       bool *parsed = NULL);
                
                bool hasKey(const std::string& key);
                
                void addBool(const std::string& key, bool value);
                const bool isBool(const std::string& key) const;
                const bool getBool(const std::string& key) const;
                
                void addInt32(const std::string& key, int32_t value);
                const bool isInt32(const std::string& key) const;
                const int32_t getInt32(const std::string& key) const;
                
                void addInt64(const std::string& key, int64_t value);
                const bool isInt64(const std::string& key) const;
                const int64_t getInt64(const std::string& key) const;
                
                void addDouble(const std::string& key, double value);
                const bool isDouble(const std::string& key) const;
                const double getDouble(const std::string& key) const;
                
                void addString(const std::string& key, const std::string& value);
                const bool isString(const std::string& key) const;
                std::string getString(const std::string& key) const;
                
                void addDataPack(const std::string& key, DataPack& value);
                const bool isDataPack(const std::string& key) const;
                DataPackUniquePtr getDataPack(const std::string& key) const;
                
                void addBinary(const std::string& key,
                               const char *s,
                               const unsigned int len);
                std::string getBinary(const std::string& key);
                void createArrayForKey(const std::string& key);
                const bool isArray(const std::string& key) const;
                void appendBool(const std::string& arr_key, bool value);
                void appendInt32(const std::string& arr_key, int32_t value);
                void appendInt64(const std::string& arr_key, int64_t value);
                void appendDouble(const std::string& arr_key, double value);
                void appendString(const std::string& arr_key, const std::string& value);
                void appendDataPack(const std::string& arr_key, DataPack& value);
                
                template<typename T>
                void addArray(const std::string& key, const std::vector<T> &value) {
                    using namespace Json;
                    root_json_object[key] = Value(arrayValue);
                    Value& array_value = root_json_object[key];
                    for(typename std::vector<T>::const_iterator it = value.begin(),
                        end = value.end();
                        it != end;
                        it++) {
                        array_value.append(*it);
                    }
                    
                }
                
                std::string toString();
                std::string toUnformattedString();
            };
        }
    }
}

#endif /* __CHAOSFramework__FF18E47_E86B_4A8B_B5CA_7EA5C789F2FB_DataPack_h */
