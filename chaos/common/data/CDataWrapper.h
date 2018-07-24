/*
 *	CDataWrapper.h
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
#ifndef CDataWrapper_H
#define CDataWrapper_H
#pragma GCC diagnostic ignored "-Wignored-attributes"

#include <chaos/common/bson/bson.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataBuffer.h>
#include <json/json.h>

#include <boost/scoped_ptr.hpp>

#include <boost/shared_ptr.hpp>
#include <utility>

namespace chaos {
    namespace common {
        namespace data {
            using namespace std;
            class CDataWrapper;
            class CDataVariant;
            /*!
             Class to read the and arry of multivalue
             */
            class CMultiTypeDataArrayWrapper;
            typedef ChaosUniquePtr<CMultiTypeDataArrayWrapper> CMultiTypeDataArrayWrapperUPtr;
            typedef ChaosSharedPtr<CMultiTypeDataArrayWrapper> CMultiTypeDataArrayWrapperSPtr;

            /*!
             Class for contain the serialization buffer
             the class deallocation will dealloc all the
             serialization buffer
             */
            class SerializationBuffer {
                size_t bSize;
                char *buffer;
            public:
                bool disposeOnDelete;
                SerializationBuffer(const char *iBuff, size_t iSize) {
                    bSize = iSize;
                    buffer = 0L;
                    disposeOnDelete = true;
                    if(iBuff && iSize){
                        buffer = (char*)malloc(iSize);
                        std::memcpy(buffer, iBuff, iSize);
                    }
                }
                ~SerializationBuffer(){
                    if(disposeOnDelete && buffer) free(buffer);
                }
                size_t getBufferLen(){return bSize;};
                const char *getBufferPtr(){return buffer;};
            };
            typedef ChaosUniquePtr<SerializationBuffer> SerializationBufferUPtr;
            typedef ChaosSharedPtr<struct _bson_t> ChaosBsonShrdPtr;
            typedef ChaosSharedPtr<struct _bson_value_t> ChaosBsonValuesShrdPtr;


            /*!
             Class that wrap the serializaiton system for data storage
             */
            class CDataWrapper {
                ChaosBsonShrdPtr bson;
                int array_index;
                ChaosBsonShrdPtr bson_tmp_array;
                CDataWrapper(const bson_t *copy_bson);
                explicit CDataWrapper(const std::string& json_document);
                int setBson(const bson_iter_t * ,const uint64_t& val);
                int setBson(const bson_iter_t * ,const int64_t& val);
                int setBson(const bson_iter_t *v ,const int32_t& val);
                int setBson(const bson_iter_t * ,const double& val);
                int setBson(const bson_iter_t *,const bool& val);
                int setBson(const bson_iter_t * ,const std::string& val);
                int setBson(const bson_iter_t * ,const void* val);
            public:
                CDataWrapper();
                explicit CDataWrapper(const char* mem_ser,
                                      uint32_t mem_size);
                explicit CDataWrapper(const char* mem_ser);
                ~CDataWrapper();
                static ChaosUniquePtr<CDataWrapper> instanceFromJson(const std::string& json_serialization);
                ChaosUniquePtr<CDataWrapper>clone();
                //add a csdata value
                void addCSDataValue(const std::string&, const CDataWrapper&);
                //get a csdata value
                ChaosUniquePtr<chaos::common::data::CDataWrapper> getCSDataValue(const std::string&) const;
                //add a string value
                //void addStringValue(const char *, const char *);
                //add a string value
                void addStringValue(const std::string&, const string&);
                //add a json value
                void addJsonValue(const std::string&, const string&);
                //add a json value
                void addJsonValue(const std::string&, Json::Value&);
                //add a strin gto array
                void appendStringToArray(const string &value);
                void appendInt32ToArray(int32_t value);
                void appendInt64ToArray(int64_t value);
                void appendDoubleToArray(double value);
                void appendBooleanToArray(bool value);
                void appendCDataWrapperToArray(const CDataWrapper& value);
                //finalize the array into a key for the current dataobject
                void finalizeArrayForKey(const std::string&);
                //get a string value
                string  getStringValue(const std::string&) const;
                const char *  getCStringValue(const std::string& key) const;
                //return a vectorvalue for a key
                CMultiTypeDataArrayWrapperSPtr getVectorValue(const std::string&) const;
                void addNullValue(const std::string&);
                //add a integer value
                void addInt32Value(const std::string&, int32_t);
                void append(const std::string& key,int32_t val);
                void append(const std::string& key,int64_t val);
                void append(const std::string& key,double val);
                void append(const std::string& key,bool val);
                void append(const std::string& key,const std::string& val);
                void append(const std::string& key,const CDataWrapper& val);
                void append(const std::string& key,const std::vector<int32_t>& val);
                void append(const std::string& key,const std::vector<int64_t>& val);
                void append(const std::string& key,const std::vector<double>& val);
                void append(const std::string& key,const std::vector<bool>& val);
                void append(const std::string& key,const std::vector<std::string>& val);
                void append(const std::string& key,const std::vector<CDataWrapper>& val);
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
                //!add a value from variant
                void addVariantValue(const std::string& key,
                                     const CDataVariant& variant_value);
                //get a integer value
                int32_t getInt32Value(const std::string& key) const;
                //get a integer value
                int64_t getInt64Value(const std::string& key) const;
                //get a integer value
                uint32_t getUInt32Value(const std::string& key) const;
                //get a integer value
                uint64_t getUInt64Value(const std::string& key) const;
                //add a integer value
                double getDoubleValue(const std::string& key) const;
                //get a bool value
                bool getBoolValue(const std::string&) const;
                //get a json value
                std::string getJsonValue(const std::string&) const;

#define THROW_TYPE_EXC(type)\
std::stringstream ss;\
ss<<"cannot get or cast to '" << #type<<"'";\
throw chaos::CException(-2, ss.str(), __PRETTY_FUNCTION__);

                template<typename T>
                int setValue(const std::string& key,const T& val){
                    bson_iter_t it;
                    bson_iter_init(&it, static_cast<bson_t*>(bson.get()));
                    if(bson_iter_find_case(&it, key.c_str()) == false)
                        return -1;
                    //const bson_value_t *v = bson_iter_value(&it);
                    return setBson(&it,val);
                }
                template<typename T>
                int setValue(const std::string& key,std::vector<T>& val){
                    bson_iter_t it;
                    bson_iter_init(&it, static_cast<bson_t*>(bson.get()));
                    if(bson_iter_find_case(&it, key.c_str()) == false)
                        return -1;
                    const bson_value_t *v = bson_iter_value(&it);
                    if(v->value_type ==BSON_TYPE_ARRAY){
                        uint32_t array_len = 0;
                        const uint8_t *array = NULL;
                        bson_t array_doc;
                        bson_iter_array(&it, &array_len, &array);
                        if (bson_init_static(&array_doc, array, array_len)) {
                            bson_iter_t iter;
                            int cnt=0;
                            int size=val.size();
                            int ret=0;
                            if(bson_iter_init(&iter, &array_doc)) {
                                while(bson_iter_next(&iter)&& (cnt<size)) {
                                    int s;
                                    //const bson_value_t *v = bson_iter_value(&it);
                                    if((s=setBson(&iter,val[cnt]))>0){
                                        ret+=s;
                                    } else {
                                        return -1;
                                    }
                                    cnt++;
                                }
                                return ret;
                            }
                        }
                    }
                    return -1;
                }
                template<typename T>
                T getValue(const std::string& key) const{
                    T v;
                    if(hasKey(key) == false) {throw chaos::CException(-1, "Key not present", __PRETTY_FUNCTION__);}
                    switch(getValueType(key)){
                        case chaos::DataType::TYPE_BOOLEAN:{
                            v = static_cast<T>(getBoolValue(key));
                            break;
                        }
                        case chaos::DataType::TYPE_INT32:{
                            v = static_cast<T>(getInt32Value(key));
                            break;
                        }
                        case chaos::DataType::TYPE_INT64:{
                            v = static_cast<T>(getInt64Value(key));
                            break;
                        }
                        case chaos::DataType::TYPE_DOUBLE:{
                            v = static_cast<T>(getDoubleValue(key));
                            break;
                        }
                        default:{
                            std::stringstream ss;
                            ss<<"cannot get key\""<<key<<"\" to type:"<<getValueType(key);
                            throw chaos::CException(-2,ss.str(),__PRETTY_FUNCTION__);
                        }
                    }
                    return v;
                }
                //return the binary data value
                const char* getBinaryValue(const std::string&, uint32_t&) const;
                chaos::DataType::BinarySubtype getBinarySubtype(const std::string& key) const;
                void addBinaryValue(const std::string& key,
                                    chaos::DataType::BinarySubtype sub_type,
                                    const char *buff,
                                    int bufLen);
                CDBufferUniquePtr getBinaryValueAsCDataBuffer(const std::string &key) const;
                //return the bson data
                SerializationBufferUPtr getBSONData() const;
                const char* getBSONRawData(int& size) const;
                const char* getBSONRawData() const;
                const int getBSONRawSize() const;
                //return the json data
                //SerializationBuffer* getJSONData();
                //return the json representation for this data wrapper
                string getJSONString() const;
                //return a compliatn json serialization
                string getCompliantJSONString() const;
                //reinitialize the object with bson data
                void setSerializedData(const char* bsonData);
                //reinitialize the object with bson data
                void setSerializedJsonData(const char* jsonData);
                //check if the key is present in data wrapper
                bool hasKey(const std::string& key) const;
                bool isVector(const std::string& key) const;
                //return all key contained into the object
                void getAllKey(ChaosStringVector& contained_key) const;
                //return all key contained into the object
                void getAllKey(ChaosStringSet& contained_key) const;
                //return all key contained into the object
                uint32_t getValueSize(const std::string& key) const;
                //! get raw value ptr address
                const char * getRawValuePtr(const std::string& key) const;
                //reset the datawrapper
                void reset();
                //append all element of an data wrapper
                void appendAllElement(CDataWrapper&);
                //!copy a key(with value) from this instance to another CDataWrapper one
                bool copyKeyTo(const std::string& key_to_copy,
                               CDataWrapper& destination);
                //!copy a key(with value) from this instance to another CDataWrapper witha new key
                bool copyKeyToNewKey(const std::string& key_to_copy,
                                     const std::string& new_key,
                                     CDataWrapper& destination);
                //!copy all key(with value) from this instance to another CDataWrapper one
                void copyAllTo(CDataWrapper& destination);
                //! Return the Hashing represetnation of the CDataWrapper
                string toHash() const;
                CDataVariant getVariantValue(const std::string& key) const;
                //---checking funciton
                bool isNullValue(const std::string& key) const;
                bool isBoolValue(const std::string& key) const;
                bool isInt32Value(const std::string& key) const;
                bool isInt64Value(const std::string& key) const;
                bool isDoubleValue(const std::string& key) const;
                bool isStringValue(const std::string& key) const;
                bool isBinaryValue(const std::string& key) const;
                bool isCDataWrapperValue(const std::string& key) const;
                bool isVectorValue(const std::string& key) const;
                bool isJsonValue(const std::string& key) const;
                chaos::DataType::DataType getValueType(const std::string& key) const;
                bool isEmpty() const;
            };
            CHAOS_DEFINE_VECTOR_FOR_TYPE(bson_value_t*, VectorBsonValues);
            /*!
             Class to read the and arry of multivalue
             */
            class CMultiTypeDataArrayWrapper {
                friend class CDataWrapper;
                const ChaosBsonShrdPtr document_shrd_ptr;
                bson_t *array_doc;
                VectorBsonValues values;
                CMultiTypeDataArrayWrapper(const ChaosBsonShrdPtr& _document_shrd_ptr,
                                           const std::string& key);
            public:
                ~CMultiTypeDataArrayWrapper();
                string getStringElementAtIndex(const int) const;
                double getDoubleElementAtIndex(const int) const;
                int32_t getInt32ElementAtIndex(const int) const;
                int64_t getInt64ElementAtIndex(const int) const;
                bool getBoolElementAtIndex(const int) const;

                ChaosUniquePtr<CDataWrapper> getCDataWrapperElementAtIndex(const int) const;
                std::string getJSONString();
                std::string getCanonicalJSONString();
                bool isStringElementAtIndex(const int) const;
                bool isDoubleElementAtIndex(const int) const;
                bool isInt32ElementAtIndex(const int) const;
                bool isInt64ElementAtIndex(const int) const;
                bool isBoolElementAtIndex(const int) const;

                bool isCDataWrapperElementAtIndex(const int) const;
                template<class T>
                T getElementAtIndex(const int pos) const{
                    if(values[pos]->value_type == BSON_TYPE_DOUBLE){
                        return static_cast<T>(values[pos]->value.v_double);
                    }
                    if(values[pos]->value_type == BSON_TYPE_INT32){
                        return static_cast<T>(values[pos]->value.v_int32);
                    }
                    if(values[pos]->value_type == BSON_TYPE_INT64){
                        return static_cast<T>(values[pos]->value.v_int64);
                    }
                    if(values[pos]->value_type == BSON_TYPE_BOOL){
                        return static_cast<T>(values[pos]->value.v_bool);
                    }
                    std::stringstream ss;
                    ss<<"type at index ["<<pos<<"] cannot convert, typeid:"<<values[pos]->value_type;
                    throw CException(1, ss.str(), __PRETTY_FUNCTION__);
                    return 0;
                }
                const char * getRawValueAtIndex(const int key,uint32_t& size) const;
                size_t size() const;
            };

#define CDW_GET_SRT_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getStringValue(k):d)
#define CDW_GET_BOOL_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getBoolValue(k):d)
#define CDW_GET_INT32_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getInt32Value(k):d)
#define CDW_GET_INT64_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getInt64Value(k):d)
#define CDW_GET_DOUBLE_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getDoubleValue(k):d)
#define CDW_CHECK_AND_SET(chk, cdw, t, k, v) if(chk){cdw->t(k,v);}
#define CDW_GET_VALUE_WITH_DEFAULT(c, k, t, d) ((c)->hasKey(k)?(c)->t(k):d)

            typedef ChaosUniquePtr<chaos::common::data::CDataWrapper> CDWUniquePtr;
            typedef ChaosSharedPtr<chaos::common::data::CDataWrapper> CDWShrdPtr;
            CHAOS_DEFINE_VECTOR_FOR_TYPE(CDWShrdPtr, VectorCDWShrdPtr);

            typedef std::pair<std::string, CDWShrdPtr> PairStrCDWShrdPtr;
            CHAOS_DEFINE_VECTOR_FOR_TYPE(PairStrCDWShrdPtr, VectorStrCDWShrdPtr);
        }
    }
}
#endif
