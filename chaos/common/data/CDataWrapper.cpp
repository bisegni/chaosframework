/*
 *	CDataWrapper.cpp
 *	!CHOAS
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
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/bson/util/json.h>

using namespace bson;

using namespace chaos;
using namespace chaos::common::data;

CMultiTypeDataArrayWrapper::CMultiTypeDataArrayWrapper(vector< BSONElement > src) {
    elementsArray = src;
}

string CMultiTypeDataArrayWrapper::getStringElementAtIndex(const int pos) {
    return elementsArray[pos].String();
}

double_t CMultiTypeDataArrayWrapper::getDoubleElementAtIndex(const int pos) {
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

CDataWrapper::CDataWrapper():bsonArrayBuilder(new BSONArrayBuilder()),bsonBuilder(new BSONObjBuilder()){
}

CDataWrapper::CDataWrapper(const char* serializationBuffer):bsonArrayBuilder(new BSONArrayBuilder()),bsonBuilder(new BSONObjBuilder()){
    bsonBuilder->appendElements(BSONObj(serializationBuffer));
}

CDataWrapper::~CDataWrapper() {
}

CDataWrapper *CDataWrapper::clone() {
    CDataWrapper *result = new CDataWrapper(bsonBuilder->asTempObj().objdata());
    return result;
}


//add a csdata value
void CDataWrapper::addCSDataValue(const char *key, CDataWrapper& csData) {
    if(csData.bsonBuilder->len()==0) return;
    bsonBuilder->append(key, csData.bsonBuilder->asTempObj());
    //reset the added object
    //csData.bsonBuilder.reset(new BSONObjBuilder());
}

//add a string value
void CDataWrapper::addStringValue(const char * key, const char * strValue) {
    bsonBuilder->append(key, strValue);
}

//add a string value
void CDataWrapper::addStringValue(const char *key, string& strValue) {
    bsonBuilder->append(key, strValue);
}

//append a strin gto an open array
void CDataWrapper::appendStringToArray(const char *stringArrayElement) {
    bsonArrayBuilder->append(stringArrayElement);
}

//append a strin gto an open array
void CDataWrapper::appendStringToArray(string& stringArrayElement) {
    bsonArrayBuilder->append(stringArrayElement);
}

//append a strin gto an open array
void CDataWrapper::appendInt32ToArray(int32_t int32ArrayElement) {
    bsonArrayBuilder->append(int32ArrayElement);
}

//append a strin gto an open array
void CDataWrapper::appendInt64ToArray(int64_t int64ArrayElement) {
    bsonArrayBuilder->append(int64ArrayElement);
}

//append a strin gto an open array
void CDataWrapper::appendDoubleToArray(double_t doubleArrayElement) {
    bsonArrayBuilder->append(doubleArrayElement);
}

//appen a CDataWrapper to an open array
void CDataWrapper::appendCDataWrapperToArray(CDataWrapper& srcDataWrapper, bool finalize) {
    bsonArrayBuilder->append(finalize?srcDataWrapper.bsonBuilder->done():srcDataWrapper.bsonBuilder->asTempObj());
}

//finalize the array into a key for the current dataobject
void CDataWrapper::finalizeArrayForKey(const char *key) {
    bsonBuilder->appendArray(key, bsonArrayBuilder->done());
    bsonArrayBuilder.reset(new BSONArrayBuilder());
}

//return a vectorvalue for a key
CMultiTypeDataArrayWrapper* CDataWrapper::getVectorValue(const char *key) {
    return new CMultiTypeDataArrayWrapper(bsonBuilder->asTempObj().getField(key).Array());
}

//add a long value
void CDataWrapper::addInt32Value(const char *key, int32_t i32Value) {
    bsonBuilder->append(key, i32Value);
}
//add a long value
void CDataWrapper::addInt32Value(const char *key, uint32_t ui32Value) {
    bsonBuilder->append(key, static_cast<int32_t>(ui32Value));
}
//add a double value
void CDataWrapper::addDoubleValue(const char *key, double dValue) {
    bsonBuilder->append(key, (double)dValue);
}

//add a integer value
void CDataWrapper::addInt64Value(const char *key, int64_t i64Value) {
    bsonBuilder->append(key, i64Value);
}

//add a integer value
void CDataWrapper::addInt64Value(const char *key, uint64_t i64Value) {
    bsonBuilder->append(key, static_cast<int64_t>(i64Value));
}

//get a csdata value
CDataWrapper *CDataWrapper::getCSDataValue(const char *key) {
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
string  CDataWrapper::getStringValue(const char *key) {
    const char * result = bsonBuilder->asTempObj().getField(key).CString();
    return string(result);
}

//get string value
const char *  CDataWrapper::getCStringValue(const char *key) {
    return bsonBuilder->asTempObj().getField(key).CString();
}
//add a integer value
int32_t CDataWrapper::getInt32Value(const char *key) {
    return bsonBuilder->asTempObj().getField(key).numberInt();
}
//add a integer value
uint32_t CDataWrapper::getUInt32Value(const char *key) {
    
    return static_cast<uint32_t>(bsonBuilder->asTempObj().getField(key).numberInt());
}
//add a integer value
int64_t CDataWrapper::getInt64Value(const char *key) {
    
    return bsonBuilder->asTempObj().getField(key).numberLong();
}
//add a integer value
uint64_t CDataWrapper::getUInt64Value(const char *key) {
    
    return static_cast<uint64_t>(bsonBuilder->asTempObj().getField(key).numberLong());
}
//add a integer value
double_t CDataWrapper::getDoubleValue(const char *key) {
    
    return bsonBuilder->asTempObj().getField(key).Double();
}

//get a bool value
bool  CDataWrapper::getBoolValue(const char *key) {
    
    return bsonBuilder->asTempObj().getField(key).Bool();
}

//set a binary data value
void CDataWrapper::addBinaryValue(const char *key, const char *buff, int bufLen) {
    bsonBuilder->appendBinData(key, bufLen, BinDataGeneral, buff);
}

//return the binary data value
const char* CDataWrapper::getBinaryValue(const char *key, int& bufLen)  {
    return bsonBuilder->asTempObj().getField(key).binData(bufLen);
}

//check if the key is present in data wrapper
bool CDataWrapper::hasKey(const char* key) {
    return bsonBuilder->asTempObj().hasElement(key);
}

//add a bool value
void CDataWrapper::addBoolValue(const char *key, bool boolValue) {
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

//reset the datawrapper
void CDataWrapper::reset() {
    bsonBuilder.reset(new BSONObjBuilder());
}

string CDataWrapper::toHash() const{
	return  bsonBuilder->asTempObj().md5();
}
