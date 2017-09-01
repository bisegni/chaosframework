/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#ifndef CDataWrapper_H
#define CDataWrapper_H

#include <chaos/common/bson/bson.h>
#include <chaos/common/chaos_constants.h>
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
                
                string getStringElementAtIndex(const int) const;
                
                
                double getDoubleElementAtIndex(const int) const;
                int32_t getInt32ElementAtIndex(const int) const;
                int64_t getInt64ElementAtIndex(const int) const ;
                CDataWrapper* getCDataWrapperElementAtIndex(const int) const;
                
                bool isStringElementAtIndex(const int) const;
                bool isDoubleElementAtIndex(const int) const;
                bool isInt32ElementAtIndex(const int) const ;
                bool isInt64ElementAtIndex(const int) const;
                bool isCDataWrapperElementAtIndex(const int) const;
                std::string getJSONString() ;

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
                
                static ChaosUniquePtr<CDataWrapper> instanceFromJson(const std::string& json_serialization);
                
                CDataWrapper *clone();
                //add a csdata value
                void addCSDataValue(const std::string&, const CDataWrapper&);
                
                //get a csdata value
                CDataWrapper *getCSDataValue(const std::string&) const;
                
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
                string  getStringValue(const std::string&) const;
                const char *  getCStringValue(const std::string& key) const;
                //return a vectorvalue for a key
                CMultiTypeDataArrayWrapper* getVectorValue(const std::string&) const;
                
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
                void addBinaryValue(const std::string& key, const char *buff, int bufLen);
                void addBinaryValue(const std::string& key,
                                    chaos::DataType::BinarySubtype sub_type,
                                    const char *buff,
                                    int bufLen);
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
                int32_t getInt32Value(const std::string& key) const;
                
                //get a integer value
                int64_t getInt64Value(const std::string& key) const;
                
                //get a integer value
                uint32_t getUInt32Value(const std::string& key) const ;
                
                //get a integer value
                uint64_t getUInt64Value(const std::string& key) const;
                
                //add a integer value
                double getDoubleValue(const std::string& key) const;
                
                //get a bool value
                bool getBoolValue(const std::string&) const;
                

                //get a json value
                std::string getJsonValue(const std::string&) const;


                template<typename T>
                T getValue(const std::string& key) const {
                    T v;
                    bsonBuilder->asTempObj().getField(key).Val(v);
                    return v;
                }
                
                template<typename T>
                T getValueWithDefault(const std::string& key, T default_value) const {
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
                const char* getBinaryValue(const std::string&, int&) const;
                
                chaos::DataType::BinarySubtype getBinarySubtype(const std::string&) const;
                ChaosUniquePtr<CDataBuffer> getBinaryValueAsCDataBuffer(const std::string &key) const;
                
                //return the bson data
                SerializationBuffer* getBSONData() const;
                
                const char* getBSONRawData(int& size) const;
                
                const int getBSONRawSize() const;
                
                //return the json data
                SerializationBuffer* getJSONData() const;
                
                //return the json representation for this data wrapper
                string getJSONString() const;
                
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
                const char * getRawValuePtr(const std::string& key) const ;
                
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
                
                CDataWrapperType getValueType(const std::string& key) const;
                
                const bool isEmpty() const;
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
