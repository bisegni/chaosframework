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
#include "../global.h"
#include "CDataWrapper.h"
using namespace chaos;
using namespace bson;

#pragma mark Public Methods For CMultiTypeDataArrayWrapper
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

#pragma mark Public Methods
CDataWrapper::CDataWrapper():bsonBuilder(new BSONObjBuilder()),bsonArrayBuilder(new BSONArrayBuilder()){
}

CDataWrapper::CDataWrapper(const char* serializationBuffer, bool bson, bool owned):bsonBuilder(new BSONObjBuilder()),bsonArrayBuilder(new BSONArrayBuilder()){
        //bsonBuilder->appendElements(BSONObj(serializationBuffer));
    setSerializedData(serializationBuffer, bson, owned);
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

    //get string value
string  CDataWrapper::getStringValue(const char *key) {
    return bsonBuilder->asTempObj().getField(key).String();
}

    //return a vectorvalue for a key
CMultiTypeDataArrayWrapper* CDataWrapper::getVectorValue(const char *key) {
    return new CMultiTypeDataArrayWrapper(bsonBuilder->asTempObj().getField(key).Array());
}

    //add a long value
void CDataWrapper::addInt32Value(const char *key, int32_t i32Value) {
    bsonBuilder->appendNumber(key, i32Value);
}

    //add a double value
void CDataWrapper::addDoubleValue(const char *key, double dValue) {
    bsonBuilder->appendNumber(key, (double)dValue);
}

    //add a integer value
void CDataWrapper::addInt64Value(const char *key, int64_t i64Value) {
#if __linux__
	bsonBuilder->appendNumber(key, (long long)i64Value);
#else
    bsonBuilder->appendNumber(key, i64Value);
#endif
}

    //add a integer value
int32_t CDataWrapper::getInt32Value(const char *key) {
    return (int32_t)bsonBuilder->asTempObj().getField(key).Int();
}

    //add a integer value
int64_t CDataWrapper::getInt64Value(const char *key) {
    return bsonBuilder->asTempObj().getField(key).Long();
}

    //add a integer value
double_t CDataWrapper::getDoubleValue(const char *key) {
    return bsonBuilder->asTempObj().getField(key).Double();
}

    //add a bool value
void CDataWrapper::addBoolValue(const char *key, bool boolValue) {
    bsonBuilder->appendBool(key, boolValue);
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
const char* CDataWrapper::getBinaryValue(const char *key, int& bufLen) {
    return bsonBuilder->asTempObj().getField(key).binData(bufLen);
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
void CDataWrapper::setSerializedData(const char* bsonData, bool bson, bool owned) {
    bsonBuilder->appendElements(bson?BSONObj(bsonData, owned):fromjson(bsonData));
}

    //append all elemento of an
void CDataWrapper::appendAllElement(CDataWrapper& srcDataWrapper) {
    bsonBuilder->appendElements(srcDataWrapper.bsonBuilder->asTempObj());
}

    //check if the key is present in data wrapper
bool CDataWrapper::hasKey(const char* key) {
    return bsonBuilder->asTempObj().hasElement(key);
}
    //reset the datawrapper
void CDataWrapper::reset() {
    bsonBuilder.reset(new BSONObjBuilder()); 
}
