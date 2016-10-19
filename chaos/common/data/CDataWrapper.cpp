/*
 *	CDataWrapper.cpp
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
#include <chaos/common/global.h>
#include <chaos/common/bson/util/json.h>
#include <chaos/common/bson/bsontypes.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/data/CDataWrapper.h>
using namespace bson;

using namespace chaos;
using namespace chaos::common::data;

#pragma mark CMultiTypeDataArrayWrapper
CMultiTypeDataArrayWrapper::CMultiTypeDataArrayWrapper(vector< BSONElement > src) {
    elementsArray = src;
}

string CMultiTypeDataArrayWrapper::getStringElementAtIndex(const int pos) {
    return elementsArray[pos].String();
}

double CMultiTypeDataArrayWrapper::getDoubleElementAtIndex(const int pos) {
    return elementsArray[pos].Double();
}
int32_t CMultiTypeDataArrayWrapper::getInt32ElementAtIndex(const int pos) {
    return elementsArray[pos].Int();
}
int64_t CMultiTypeDataArrayWrapper::getint64ElementAtIndex(const int pos) {
    return elementsArray[pos].Long();
}

CDataWrapper* CMultiTypeDataArrayWrapper::getCDataWrapperElementAtIndex(const int pos) {
    return new CDataWrapper(elementsArray[pos].embeddedObject().objdata());
}
vector<BSONElement>::size_type CMultiTypeDataArrayWrapper::size() const{
    return elementsArray.size();
}

#pragma mark CDataWrapper
CDataWrapper::CDataWrapper():
bsonArrayBuilder(new BSONArrayBuilder()),
bsonBuilder(new BSONObjBuilder()){}

CDataWrapper::CDataWrapper(const char* serializationBuffer):
bsonArrayBuilder(new BSONArrayBuilder()),
bsonBuilder(new BSONObjBuilder()){
    bsonBuilder->appendElements(BSONObj(serializationBuffer));
}

CDataWrapper::~CDataWrapper() {}

CDataWrapper *CDataWrapper::clone() {
    CDataWrapper *result = new CDataWrapper(bsonBuilder->asTempObj().objdata());
    return result;
}


//add a csdata value
void CDataWrapper::addCSDataValue(const std::string& key, CDataWrapper& csData) {
    if(csData.bsonBuilder->len()==0) return;
    bsonBuilder->append(key, csData.bsonBuilder->asTempObj());
}

//add a string value
void CDataWrapper::addStringValue(const std::string& key, const string& strValue) {
    try{
        bsonBuilder->append(key, strValue);
    }catch(bson::MsgAssertionException& ex) {
        LERR_<< "[" <<__PRETTY_FUNCTION__<< "]" << ex.what();
    }
}

//append a strin gto an open array
void CDataWrapper::appendStringToArray(const char * stringArrayElement) {
    bsonArrayBuilder->append(stringArrayElement);
}

//append a strin gto an open array
void CDataWrapper::appendStringToArray(const string& stringArrayElement) {
    bsonArrayBuilder->append(stringArrayElement);
}

//append a strin gto an open array
void CDataWrapper::appendInt32ToArray(int32_t int32ArrayElement) {
    bsonArrayBuilder->append(int32ArrayElement);
}

//append a strin gto an open array
void CDataWrapper::appendInt64ToArray(int64_t int64ArrayElement) {
    bsonArrayBuilder->append((long long)int64ArrayElement);
}

//append a strin gto an open array
void CDataWrapper::appendDoubleToArray(double doubleArrayElement) {
    // TODO: all std int or NOT
    bsonArrayBuilder->append(doubleArrayElement);
}

//appen a CDataWrapper to an open array
void CDataWrapper::appendCDataWrapperToArray(CDataWrapper& srcDataWrapper, bool finalize) {
    bsonArrayBuilder->append(finalize?srcDataWrapper.bsonBuilder->done():srcDataWrapper.bsonBuilder->asTempObj());
}

//finalize the array into a key for the current dataobject
void CDataWrapper::finalizeArrayForKey(const std::string& key) {
    try{
        bsonBuilder->appendArray(key, bsonArrayBuilder->done());
        bsonArrayBuilder.reset(new BSONArrayBuilder());
    } catch(bson::MsgAssertionException& ass_except) {
        throw CException(-1, ass_except.what(), __PRETTY_FUNCTION__);
    }
}

//return a vectorvalue for a key
CMultiTypeDataArrayWrapper* CDataWrapper::getVectorValue(const std::string& key) {
    try {
        if(bsonBuilder->asTempObj().hasElement(key))
            return new CMultiTypeDataArrayWrapper(bsonBuilder->asTempObj().getField(key).Array());
        
    } catch (...) {
    }
    return NULL;
}

void CDataWrapper::addNullValue(const std::string& key) {
    bsonBuilder->appendNull(key);
}
//add a long value
void CDataWrapper::addInt32Value(const std::string& key, int32_t i32Value) {
    bsonBuilder->append(key, i32Value);
}
//add a long value
void CDataWrapper::addInt32Value(const std::string& key, uint32_t ui32Value) {
    bsonBuilder->append(key, static_cast<int32_t>(ui32Value));
}
//add a double value
void CDataWrapper::addDoubleValue(const std::string& key, double dValue) {
    bsonBuilder->append(key, (double)dValue);
}

//add a integer value
void CDataWrapper::addInt64Value(const std::string& key, int64_t i64Value) {
    bsonBuilder->append(key, (long long)i64Value);
}

//add a integer value
void CDataWrapper::addInt64Value(const std::string& key, uint64_t i64Value) {
    bsonBuilder->append(key, (long long)static_cast<int64_t>(i64Value));
}

//get a csdata value
CDataWrapper *CDataWrapper::getCSDataValue(const std::string& key) {
    //allocate the pointer for the result
    CDataWrapper *result = new CDataWrapper();
    if(result){
        //get the subobject
        BSONObj subObject = bsonBuilder->asTempObj().getObjectField(key);
        //set the serialization data in resul datawrapper
        result->setSerializedData(subObject.objdata());
    }
    return result;
}

//get string value
string  CDataWrapper::getStringValue(const std::string& key) {
    return bsonBuilder->asTempObj().getField(key).String();
}

//get string value
const char *  CDataWrapper::getCStringValue(const std::string& key) {
    //return bsonBuilder->asTempObj().getField(key).String().c_str();
    return getRawValuePtr(key);
}

//add a integer value
int32_t CDataWrapper::getInt32Value(const std::string& key) {
    return bsonBuilder->asTempObj().getField(key).numberInt();
}
//add a integer value
uint32_t CDataWrapper::getUInt32Value(const std::string& key) {
    
    return static_cast<uint32_t>(bsonBuilder->asTempObj().getField(key).numberInt());
}
//add a integer value
int64_t CDataWrapper::getInt64Value(const std::string& key) {
    
    return bsonBuilder->asTempObj().getField(key).numberLong();
}
//add a integer value
uint64_t CDataWrapper::getUInt64Value(const std::string& key) {
    
    return static_cast<uint64_t>(bsonBuilder->asTempObj().getField(key).numberLong());
}
//add a integer value
double CDataWrapper::getDoubleValue(const std::string& key) {
    
    return bsonBuilder->asTempObj().getField(key).Double();
}

//get a bool value
bool  CDataWrapper::getBoolValue(const std::string& key) {
    
    return bsonBuilder->asTempObj().getField(key).Bool();
}

//set a binary data value
void CDataWrapper::addBinaryValue(const std::string& key, const char *buff, int bufLen) {
    bsonBuilder->appendBinData(key, bufLen, BinDataGeneral, buff);
}

void CDataWrapper::addVariantValue(const std::string& key,
                                   const CDataVariant& variant_value) {
    //create variant using the typed data
    switch (variant_value.getType()) {
        case DataType::TYPE_BOOLEAN:
            addBoolValue(key, variant_value.asBool());
            break;
        case DataType::TYPE_INT32:
            addInt32Value(key, variant_value.asInt32());
            break;
        case DataType::TYPE_INT64:
            addInt64Value(key, variant_value.asInt64());
            break;
        case DataType::TYPE_DOUBLE:
            addDoubleValue(key, variant_value.asDouble());
            break;
        case DataType::TYPE_STRING:
            addStringValue(key, variant_value.asString());
            break;
        case DataType::TYPE_BYTEARRAY:
            addBinaryValue(key,
                           variant_value.asCDataBuffer()->getBuffer(),
                           variant_value.asCDataBuffer()->getBufferSize());
            break;
    }
    
}

//return the binary data value
const char* CDataWrapper::getBinaryValue(const std::string& key, int& bufLen)  {
    return bsonBuilder->asTempObj().getField(key).binData(bufLen);
}

auto_ptr<CDataBuffer> CDataWrapper::getBinaryValueAsCDataBuffer(const std::string &key) {
    int bufLen = 0;
    const char* buffer = bsonBuilder->asTempObj().getField(key).binData(bufLen);
    return auto_ptr<CDataBuffer>(new CDataBuffer(buffer, bufLen, true));
}

//check if the key is present in data wrapper
bool CDataWrapper::hasKey(const std::string& key) {
    return bsonBuilder->asTempObj().hasElement(key);
}

bool CDataWrapper::isVector(const std::string& key) {
    return bsonBuilder->asTempObj().getField(key).type() == bson::Array;
}

//return all key contained into the object
void CDataWrapper::getAllKey(ChaosStringVector& contained_key) {
    BSONObjIterator obj_iterator(bsonBuilder->asTempObj());
    while(obj_iterator.more()) {
        //we have another key
        BSONElement element = obj_iterator.next();
        
        //add key to vector
        contained_key.push_back(element.fieldNameStringData().toString());
    }
}

//return all key contained into the object
void CDataWrapper::getAllKey(ChaosStringSet& contained_key) {
    BSONObjIterator obj_iterator(bsonBuilder->asTempObj());
    while(obj_iterator.more()) {
        //we have another key
        BSONElement element = obj_iterator.next();
        
        //add key to vector
        contained_key.insert(element.fieldNameStringData().toString());
    }
}

//return all key contained into the object
uint32_t CDataWrapper::getValueSize(const std::string& key) {
    BSONElement ele  = bsonBuilder->asTempObj().getField(key);
    int bsize = 0;
    switch(ele.type()) {
        case NumberLong:
        case NumberInt:
        case Bool:
        case NumberDouble:
            return ele.valuesize();
        case String:
            return ele.valuestrsize();
        case BinData:
            ele.binData(bsize);
            return bsize;
        default:
            break;
    }
    return 0;
}

//! return the raw value ptr address
const char * CDataWrapper::getRawValuePtr(const std::string& key) {
    BSONElement ele  = bsonBuilder->asTempObj().getField(key);
    int bsize = 0;
    switch(ele.type()) {
        case NumberLong:
        case NumberInt:
        case Bool:
        case NumberDouble:
            return ele.value();
        case String:
            return ele.valuestrsafe();
        case BinData:
            return ele.binDataClean(bsize);
        default:
            break;
    }
    return NULL;
}

//add a bool value
void CDataWrapper::addBoolValue(const std::string& key, bool boolValue) {
    bsonBuilder->append(key, boolValue);
}

/*
 Return the Serialized buffer object taht contain the memory,
 the requester of this method shuld be deallocate the object
 */
SerializationBuffer* CDataWrapper::getBSONData(){
    //finalize the bson object
    if( bsonBuilder->len()==0 ) return NULL;
    
    //get the object
    BSONObj bsonObject = bsonBuilder->asTempObj();
    //we have some data
    SerializationBuffer *result = new SerializationBuffer(bsonObject.objdata(), bsonObject.objsize());
    //recreate bson builder for next fill
    return result;
}

/*
 Return the pointer of the data and the size. th eownership of data remain
 of the CDataWrapper
 */
const char* CDataWrapper::getBSONRawData(int& size) const {
    //finalize the bson object
    if( bsonBuilder->len()==0 ) return NULL;
    
    //get the object
    BSONObj bsonObject = bsonBuilder->asTempObj();
    //we have some data
    size = bsonObject.objsize();
    //recreate bson builder for next fill
    return bsonObject.objdata();
}

const int CDataWrapper::getBSONRawSize() const {
    //finalize the bson object
    if( bsonBuilder->len()==0 ) return NULL;
    
    return bsonBuilder->asTempObj().objsize();
}

//return the json data
SerializationBuffer* CDataWrapper::getJSONData() {
    //finalize the bson object
    if( bsonBuilder->len()==0 ) return 0L;
    
    //get the object
    BSONObj bsonObject = bsonBuilder->asTempObj();
    //return bsonObject.toString();
    string jsonString = bsonObject.jsonString();
    //recreate bson builder for next fill
    //bsonBuilder.reset(new BSONObjBuilder());
    return new SerializationBuffer(jsonString.c_str(), jsonString.size());
}

//return the json data
string CDataWrapper::getJSONString() {
    //finalize the bson object
    if( bsonBuilder->len()==0 ) return string("");
    
    return bsonBuilder->asTempObj().jsonString();
}
//reinitialize the object with bson data
void CDataWrapper::setSerializedData(const char* bsonData) {
    //bsonBuilder->appendElements(bson?BSONObj(bsonData):fromjson(bsonData));
    bsonBuilder->appendElements(BSONObj(bsonData));
}

//reinitialize the object with bson data
void CDataWrapper::setSerializedJsonData(const char* jsonData) {
    bsonBuilder->appendElements(fromjson(jsonData));
}

//append all elemento of an
void CDataWrapper::appendAllElement(CDataWrapper& srcDataWrapper) {
    bsonBuilder->appendElements(srcDataWrapper.bsonBuilder->asTempObj());
}

bool CDataWrapper::copyKeyTo(const std::string& key_to_copy,
                             CDataWrapper& destination) {
    bool result = false;
    if((result = bsonBuilder->asTempObj().hasElement(key_to_copy))){
        //we can copy
        destination.bsonBuilder->append(bsonBuilder->asTempObj().getField(key_to_copy));
    }
    return result;
}

//!copy a key(with value) from this instance to another CDataWrapper witha new key
bool CDataWrapper::copyKeyToNewKey(const std::string& key_to_copy,
                                   const std::string& new_key,
                                   CDataWrapper& destination) {
    bool result = false;
    if((result = bsonBuilder->asTempObj().hasElement(key_to_copy))){
        //we can copy
        destination.bsonBuilder->appendAs(bsonBuilder->asTempObj().getField(key_to_copy), new_key);
    }
    return result;
}

void CDataWrapper::copyAllTo(CDataWrapper& destination) {
    std::vector<BSONElement> all_element;
    bsonBuilder->asTempObj().elems(all_element);
    for(std::vector<BSONElement>::iterator it = all_element.begin();
        it != all_element.end();
        it++) {
        destination.bsonBuilder->append(*it);
    }
}

//reset the datawrapper
void CDataWrapper::reset() {
    bsonBuilder.reset(new BSONObjBuilder());
}

string CDataWrapper::toHash() const{
    return  bsonBuilder->asTempObj().md5();
}

CDataVariant CDataWrapper::getVariantValue(const std::string& key) {
    //check if key is present
    if(!hasKey(key)) return CDataVariant();
    
    //create variant using the typed data
    switch (getValueType(key)) {
        case CDataWrapperTypeBool:
            return CDataVariant(getBoolValue(key));
            break;
        case CDataWrapperTypeInt32:
            return CDataVariant(getInt32Value(key));
            break;
        case CDataWrapperTypeInt64:
            return CDataVariant(getInt64Value(key));
            break;
        case CDataWrapperTypeDouble:
            return CDataVariant(getDoubleValue(key));
            break;
        case CDataWrapperTypeString:
            return CDataVariant(getStringValue(key));
            break;
        case CDataWrapperTypeBinary:
            return CDataVariant(getBinaryValueAsCDataBuffer(key).release());
            break;
        default:
            return CDataVariant();
            break;
    }
}

//------------------------checking utility

bool CDataWrapper::isNullValue(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).isNull();
}

bool CDataWrapper::isBoolValue(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).isBoolean();
}

bool CDataWrapper::isInt32Value(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).type()==NumberInt;
}

bool CDataWrapper::isInt64Value(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).type()==NumberLong;
}

bool CDataWrapper::isDoubleValue(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).type()==NumberDouble;
}

bool CDataWrapper::isStringValue(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).type()==String;
}

bool CDataWrapper::isBinaryValue(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).type()==BinData;
}

bool CDataWrapper::isCDataWrapperValue(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).type()==Object;
}

bool CDataWrapper::isVectorValue(const std::string& key){
    return bsonBuilder->asTempObj().getField(key).type()==Array;
}

CDataWrapperType CDataWrapper::getValueType(const std::string& key) {
    CDataWrapperType result = CDataWrapperTypeNoType;
    switch(bsonBuilder->asTempObj().getField(key).type()) {
        case Array:
            result = CDataWrapperTypeVector;
            break;
        case Object:
            result = CDataWrapperTypeObject;
            break;
        case BinData:
            result = CDataWrapperTypeBinary;
            break;
        case String:
            result = CDataWrapperTypeString;
            break;
        case NumberDouble:
            result = CDataWrapperTypeDouble;
            break;
        case NumberInt:
            result = CDataWrapperTypeInt32;
            break;
        case NumberLong:
            result = CDataWrapperTypeInt64;
            break;
        case Bool:
            result = CDataWrapperTypeBool;
            break;
        case jstNULL:
            result = CDataWrapperTypeNULL;
            break;
        default:
            break;
            
    }
    return result;
}
