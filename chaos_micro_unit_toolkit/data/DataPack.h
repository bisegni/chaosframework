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

namespace chaos {
    namespace micro_unit_toolkit {
        namespace data {
            //! main singleton lab entrypoint
            class DataPack {
                //data pointer is not owned by this object
                Json::Value root_json_object;
                DataPack(const Json::Value& src_root_json_object);
            public:
                DataPack();
                virtual ~DataPack();
                static ChaosUniquePtr<DataPack> newFromBuffer(const char *data,
                                                              const size_t data_len,
                                                              bool *parsed = NULL);
                
                void addBoolValue(const std::string& key, bool value);
                const bool getBool(const std::string& key) const;
                
                void addInt32Value(const std::string& key, int32_t value);
                const int32_t getInt32Value(const std::string& key) const;
                
                void addInt64Value(const std::string& key, int64_t value);
                const int64_t getInt64Value(const std::string& key) const;
                
                void addDoubleValue(const std::string& key, double value);
                const double getDouble(const std::string& key) const;
                
                void addStringValue(const std::string& key, const std::string& value);
                std::string getString(const std::string& key) const;
                
                void addDataPackValue(const std::string& key, DataPack& value);
                const ChaosUniquePtr<DataPack> getDataPackValue(const std::string& key) const;
                
                void createArrayForKey(const std::string& key);
                void appendBoolValue(const std::string& arr_key, bool value);
                void appendInt32Value(const std::string& arr_key, int32_t value);
                void appendInt64Value(const std::string& arr_key, int64_t value);
                void appendDoubleValue(const std::string& arr_key, double value);
                void appendStringValue(const std::string& arr_key, const std::string& value);
                void appendDataPackValue(const std::string& arr_key, DataPack& value);
                
                std::string toString();
            };
        }
    }
}

#endif /* __CHAOSFramework__FF18E47_E86B_4A8B_B5CA_7EA5C789F2FB_DataPack_h */
