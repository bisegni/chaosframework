/*
 *	DataPack.h
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
#ifndef DataPack_H
#define DataPack_H

#pragma GCC diagnostic ignored "-Wignored-attributes"

#include <chaos_micro_unit_toolkit/external_lib/bson/bson.h>
#include <chaos_micro_unit_toolkit/micro_unit_toolkit_types.h>

#include <set>
#include <string>
#include <vector>
namespace chaos {
    namespace micro_unit_toolkit {
        namespace data {
            
            typedef enum DataPackType {
                DataPackTypeNoType,
                DataPackTypeNULL,
                DataPackTypeBool,
                DataPackTypeInt32,
                DataPackTypeInt64,
                DataPackTypeDouble,
                DataPackTypeString,
                DataPackTypeBinary,
                DataPackTypeObject,
                DataPackTypeVector
            } DataPackType;
            
            class DataPack;
            /*!
             Class to read the and arry of multivalue
             */
            class CMultiTypeDataArrayWrapper;
            
            typedef micro_unit_toolkit::ChaosSharedPtr<bson_t> ChaosBsonShrdPtr;
            typedef micro_unit_toolkit::ChaosUniquePtr<DataPack> CDWUniquePtr;
            typedef micro_unit_toolkit::ChaosSharedPtr<DataPack> CDWShrdPtr;
            /*!
             Class that wrap the serializaiton system for data storage
             */
            class DataPack {
                ChaosBsonShrdPtr bson;
                
                int array_index;
                ChaosBsonShrdPtr bson_tmp_array;
                DataPack(const bson_t *copy_bson);
                explicit DataPack(const std::string& json_document);
            public:
                
                DataPack();
                explicit DataPack(const char* mem_ser,
                                      uint32_t mem_size);
                
                explicit DataPack(const char* mem_ser);
                ~DataPack();
                
                static CDWUniquePtr instanceFromJson(const std::string& json_serialization);
                
                DataPack *clone();
                
                //add a csdata value
                void addCDWValue(const std::string& key,
                                 const DataPack& value);
                CDWUniquePtr getCDWValue(const std::string& key) const;
                bool isCDWValue(const std::string& key) const;
                
                //add a string value
                //void addStringValue(const char *, const char *);
                
                //add a string value
                void addStringValue(const std::string&, const std::string&);
                void appendStringToArray(const std::string &value);
                std::string  getStringValue(const std::string&) const;
                
                
                void appendInt32ToArray(int32_t value);
                void appendInt64ToArray(int64_t value);
                void appendDoubleToArray(double value);
                void appendDataPackToArray(DataPack& value);
                //finalize the array into a key for the current dataobject
                void finalizeArrayForKey(const std::string&);
                
                //get a string value
                const char *  getCStringValue(const std::string& key) const;
                //return a vectorvalue for a key
                CMultiTypeDataArrayWrapper* getVectorValue(const std::string&) const;
                
                void addNullValue(const std::string&);
                
                //add a integer value
                void addInt32Value(const std::string&, int32_t);
                
                //add a integer value
                void addInt32Value(const std::string&, uint32_t);
                //add a integer value
                void addInt64Value(const std::string&, int64_t);
                //add a integer value
                void addInt64Value(const std::string&, uint64_t);
                
                //add a double value
                void addDoubleValue(const std::string&key, double dValue);
                
                //add a bool value
                void addBoolValue(const std::string&, bool);
                
                //set a binary data value
                void addBinaryValue(const std::string&, const char *, int);
                const char* getBinaryValue(const std::string& key, uint32_t& bufLen) const;
                //get a integer value
                int32_t getInt32Value(const std::string& key,
                                      int32_t default_value = 0) const;
                
                //get a integer value
                int64_t getInt64Value(const std::string& key,
                                      int64_t default_value = 0) const;
                
                //get a integer value
                uint32_t getUInt32Value(const std::string& key,
                                        uint32_t default_value = 0) const;
                
                //get a integer value
                uint64_t getUInt64Value(const std::string& key,
                                        uint64_t default_value = 0) const;
                
                //add a integer value
                double getDoubleValue(const std::string& key,
                                      double default_value = 0) const;
                
                //get a bool value
                bool getBoolValue(const std::string&,
                                  bool default_value = 0) const;
                
                
                template<typename T>
                T getValue(const std::string& key) const{
                    T v;
                    bson_iter_t element_found;
                    bson_iter_init(&element_found, bson.get());
                    if(bson_iter_find_case(&element_found, key.c_str())){
                        v = static_cast<T>(element_found.raw);
                    }
                    return v;
                }
                
                const char* getBSONRawData() const;
                
                const int getBSONRawSize() const;
                
                //return the json representation for this data wrapper
                std::string toString() const;
                
                //reinitialize the object with bson data
                void setSerializedData(const char* bsonData);
                
                //reinitialize the object with bson data
                void setSerializedJsonData(const char* jsonData);
                
                //check if the key is present in data wrapper
                bool hasKey(const std::string& key) const;
                
                bool isVector(const std::string& key) const;
                
                //return all key contained into the object
                void getAllKey(std::vector<std::string>& contained_key) const;
                
                //return all key contained into the object
                void getAllKey(std::set<std::string>& contained_key) const;
                
                //return all key contained into the object
                uint32_t getValueSize(const std::string& key) const;
                
                //! get raw value ptr address
                const char * getRawValuePtr(const std::string& key) const;
                
                //reset the datawrapper
                void reset();
                
                //append all element of an data wrapper
                void appendAllElement(DataPack&);
                
                //!copy a key(with value) from this instance to another DataPack one
                bool copyKeyTo(const std::string& key_to_copy,
                               DataPack& destination);
                
                //!copy a key(with value) from this instance to another DataPack witha new key
                bool copyKeyToNewKey(const std::string& key_to_copy,
                                     const std::string& new_key,
                                     DataPack& destination);
                
                //!copy all key(with value) from this instance to another DataPack one
                void copyAllTo(DataPack& destination);
                
                //! Return the Hashing represetnation of the DataPack
                std::string toHash() const;
                
                //---checking funciton
                
                bool isNullValue(const std::string& key) const;
                
                bool isBoolValue(const std::string& key) const;
                
                bool isInt32Value(const std::string& key) const;
                
                bool isInt64Value(const std::string& key) const;
                
                bool isDoubleValue(const std::string& key) const;
                
                bool isStringValue(const std::string& key) const;
                
                bool isBinaryValue(const std::string& key) const;

                
                bool isVectorValue(const std::string& key) const;
                
                bool isJsonValue(const std::string& key) const;
                
                DataPackType getValueType(const std::string& key) const;
                
                bool isEmpty() const;
            };
            
            typedef std::vector<bson_value_t> VectorBsonValues;
            typedef std::vector<bson_value_t>::iterator VectorBsonValuesIterator;
            /*!
             Class to read the and arry of multivalue
             */
            class CMultiTypeDataArrayWrapper {
                friend class DataPack;
                const ChaosBsonShrdPtr document_shrd_ptr;
                bson_t array_doc;
                VectorBsonValues values;
                CMultiTypeDataArrayWrapper(const ChaosBsonShrdPtr& _document_shrd_ptr,
                                           const std::string& key);
            public:
                ~CMultiTypeDataArrayWrapper();
                std::string getStringElementAtIndex(const int) const;
                
                
                double getDoubleElementAtIndex(const int) const;
                int32_t getInt32ElementAtIndex(const int) const;
                int64_t getint64ElementAtIndex(const int) const;
                DataPack* getDataPackElementAtIndex(const int) const;
                
                bool isStringElementAtIndex(const int) const;
                bool isDoubleElementAtIndex(const int) const;
                bool isInt32ElementAtIndex(const int) const;
                bool isInt64ElementAtIndex(const int) const;
                bool isDataPackElementAtIndex(const int) const;
                
                size_t size() const;
            };
            
#define CDW_GET_SRT_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getStringValue(k):d)
#define CDW_GET_BOOL_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getBoolValue(k):d)
#define CDW_GET_INT32_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getInt32Value(k):d)
#define CDW_GET_INT64_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getInt64Value(k):d)
#define CDW_GET_DOUBLE_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getDoubleValue(k):d)
#define CDW_CHECK_AND_SET(chk, cdw, t, k, v) if(chk){cdw->t(k,v);}
#define CDW_GET_VALUE_WITH_DEFAULT(c, k, t, d) ((c)->hasKey(k)?(c)->t(k):d)
            


        }
    }
}
#endif
