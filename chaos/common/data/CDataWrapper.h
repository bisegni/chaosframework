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

#include <chaos/common/bson/bson.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataBuffer.h>
#include <json/json.h>

#include <boost/scoped_ptr.hpp>

#include <boost/shared_ptr.hpp>

#include <utility>

namespace chaos {
    namespace common {
        namespace data {
            using namespace std;
            using namespace bson;
            
            typedef enum CDataWrapperType {
                CDataWrapperTypeNoType,
                CDataWrapperTypeNULL,
                CDataWrapperTypeBool,
                CDataWrapperTypeInt32,
                CDataWrapperTypeInt64,
                CDataWrapperTypeDouble,
                CDataWrapperTypeString,
                CDataWrapperTypeBinary,
                CDataWrapperTypeObject,
                CDataWrapperTypeVector
            } CDataWrapperType;
            
            class CDataWrapper;
            class CDataVariant;
            /*!
             Class to read the and arry of multivalue
             */
            class CMultiTypeDataArrayWrapper {
                vector< BSONElement > elementsArray;
                
            public:
                CMultiTypeDataArrayWrapper(vector< BSONElement > src);
                
                string getStringElementAtIndex(const int);
                double getDoubleElementAtIndex(const int);
                int32_t getInt32ElementAtIndex(const int);
                int64_t getint64ElementAtIndex(const int);
                CDataWrapper* getCDataWrapperElementAtIndex(const int);
                
                bool isStringElementAtIndex(const int);
                bool isDoubleElementAtIndex(const int);
                bool isInt32ElementAtIndex(const int);
                bool isInt64ElementAtIndex(const int);
                bool isCDataWrapperElementAtIndex(const int);
                
                vector<BSONElement>::size_type size() const;
            };
            
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
            
            /*!
             Class that wrap the serializaiton system for data storage
             */
            class CDataWrapper/*: public CPoolMemoryObject<CDataWrapper> */{
                boost::scoped_ptr<BSONArrayBuilder> bsonArrayBuilder;
                
            protected:
                boost::scoped_ptr<BSONObjBuilder> bsonBuilder;
                
            public:
                
                CDataWrapper();
                CDataWrapper(const char* serializationBuffer);
                ~CDataWrapper();
                
                CDataWrapper *clone();
                //add a csdata value
                void addCSDataValue(const std::string&, const CDataWrapper&);
                
                //get a csdata value
                CDataWrapper *getCSDataValue(const std::string&);
                
                //add a string value
                //void addStringValue(const char *, const char *);
                
                //add a string value
                void addStringValue(const std::string&, const string&);
                
                //add a json value
                void addJsonValue(const std::string&, const string&);
                
                //add a json value
                void addJsonValue(const std::string&, Json::Value&);
                
                //add a string to array
                void appendStringToArray(const char *);
                
                //add a strin gto array
                void appendStringToArray(const string &);
                void appendInt32ToArray(int32_t int32ArrayElement);
                void appendInt64ToArray(int64_t int64ArrayElement);
                void appendDoubleToArray(double doubleArrayElement);
                void appendCDataWrapperToArray(CDataWrapper& srcDataWrapper, bool finalize=false);
                //finalize the array into a key for the current dataobject
                void finalizeArrayForKey(const std::string&);
                
                //get a string value
                string  getStringValue(const std::string&);
                const char *  getCStringValue(const std::string& key);
                //return a vectorvalue for a key
                CMultiTypeDataArrayWrapper* getVectorValue(const std::string&);
                
                void addNullValue(const std::string&);
                
                //add a integer value
                void addInt32Value(const std::string&, int32_t);
                
                template <typename T>
                void addValue(const std::string& key,T val){
                    bsonBuilder->append(key, static_cast<T>(val));
                    
                }
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
                template<typename T>
                void addVectorValue(const std::string &key,T v[]){
                	for(int cnt=0;cnt<sizeof(v)/sizeof(T);cnt++){
                		bsonArrayBuilder->append(v[cnt]);
                	}
                	finalizeArrayForKey(key);
                }
                
                template<typename T>
                void addVectorValue(const std::string &key,std::vector<T> v){
                                	for(typename std::vector<T>::iterator cnt=v.begin();cnt!=v.end();cnt++){
                                		bsonArrayBuilder->append(*cnt);
                                	}
                                	finalizeArrayForKey(key);
                                }
                //!add a value from variant
                void addVariantValue(const std::string& key,
                                     const CDataVariant& variant_value);
                
                //get a integer value
                int32_t getInt32Value(const std::string& key);
                
                //get a integer value
                int64_t getInt64Value(const std::string& key);
                
                //get a integer value
                uint32_t getUInt32Value(const std::string& key);
                
                //get a integer value
                uint64_t getUInt64Value(const std::string& key);
                
                //add a integer value
                double getDoubleValue(const std::string& key);
                
                //get a bool value
                bool getBoolValue(const std::string&);
                

                //get a json value
                std::string getJsonValue(const std::string&);


                template<typename T>
                T getValue(const std::string& key) {
                    T v;
                    bsonBuilder->asTempObj().getField(key).Val(v);
                    return v;
                }
                
                template<typename T>
                T getValueWithDefault(const std::string& key, T default_value) {
                    T v;
                    BSONObj o = bsonBuilder->asTempObj();
                    if(o.hasElement(key)) {
                        o.getField(key).Val(v);
                    } else {
                        v = default_value;
                    }
                    return v;
                }
                
                //return the binary data value
                const char* getBinaryValue(const std::string&, int&);
                
                std::auto_ptr<CDataBuffer> getBinaryValueAsCDataBuffer(const std::string &key);
                
                //return the bson data
                SerializationBuffer* getBSONData();
                
                const char* getBSONRawData(int& size) const;
                
                const int getBSONRawSize() const;
                
                //return the json data
                SerializationBuffer* getJSONData();
                
                //return the json representation for this data wrapper
                string getJSONString();
                
                //reinitialize the object with bson data
                void setSerializedData(const char* bsonData);
                
                //reinitialize the object with bson data
                void setSerializedJsonData(const char* jsonData);
                
                //check if the key is present in data wrapper
                bool hasKey(const std::string& key);
                
                bool isVector(const std::string& key);
                
                //return all key contained into the object
                void getAllKey(ChaosStringVector& contained_key);
                
                //return all key contained into the object
                void getAllKey(ChaosStringSet& contained_key);
                
                //return all key contained into the object
                uint32_t getValueSize(const std::string& key);
                
                //! get raw value ptr address
                const char * getRawValuePtr(const std::string& key);
                
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
                
                CDataVariant getVariantValue(const std::string& key);
                
                //---checking funciton
                
                bool isNullValue(const std::string& key);
                
                bool isBoolValue(const std::string& key);
                
                bool isInt32Value(const std::string& key);
                
                bool isInt64Value(const std::string& key);
                
                bool isDoubleValue(const std::string& key);
                
                bool isStringValue(const std::string& key);
                
                bool isBinaryValue(const std::string& key);
                
                bool isCDataWrapperValue(const std::string& key);
                
                bool isVectorValue(const std::string& key);
                
                bool isJsonValue(const std::string& key);
                
                CDataWrapperType getValueType(const std::string& key);
            };
            
            //! MutableCDataWrapper for field update
            /*! \class MutableCDataWrapper
             This implementation permit to modify the existent field value
             */
            class MutableCDataWrapper : public CDataWrapper {
                
            public:
                
                void updateStringValue(const std::string& key, string& newvalue) {
                    BSONElement element = bsonBuilder->asTempObj()[key];
                    if(element.String().size() != newvalue.size()) return;
                    //*reinterpret_cast< char* >( element.value() ) = newvalue.c_str;
                }
                
                void updateInt32Value(const std::string& key, int32_t newvalue) {
                    BSONElement element = bsonBuilder->asTempObj()[key];
                    *reinterpret_cast< int32_t* >( (char*)element.value() ) = newvalue;
                }
                
                void updateInt64Value(const std::string& key, int64_t newvalue) {
                    BSONElement element = bsonBuilder->asTempObj()[key];
                    *reinterpret_cast< int64_t* >( (char*)element.value() ) = newvalue;
                }
                
                void updateDoubleValue(const std::string& key, double newvalue) {
                    BSONElement element = bsonBuilder->asTempObj()[key];
                    *reinterpret_cast< double* >( (char*)element.value() ) = newvalue;
                }
            };
            
#define CDW_GET_SRT_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getStringValue(k):d)
#define CDW_GET_BOOL_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getBoolValue(k):d)
#define CDW_GET_INT32_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getInt32Value(k):d)
#define CDW_GET_INT64_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getInt64Value(k):d)
#define CDW_GET_DOUBLE_WITH_DEFAULT(c, k, d) ((c)->hasKey(k)?(c)->getDoubleValue(k):d)
#define CDW_GET_VALUE_WITH_DEFAULT(c, k, t, d) ((c)->hasKey(k)?(c)->t(k):d)
            
            typedef boost::shared_ptr<chaos::common::data::CDataWrapper> CDWShrdPtr;
            CHAOS_DEFINE_VECTOR_FOR_TYPE(CDWShrdPtr, VectorCDWShrdPtr);
            
            typedef std::pair<std::string, CDWShrdPtr> PairStrCDWShrdPtr;
            CHAOS_DEFINE_VECTOR_FOR_TYPE(PairStrCDWShrdPtr, VectorStrCDWShrdPtr);
        }
    }
}
#endif
