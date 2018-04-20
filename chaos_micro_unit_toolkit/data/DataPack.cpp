/*
 *	DataPack.cpp
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
#include <chaos_micro_unit_toolkit/data/DataPack.h>
#include <cassert>
using namespace chaos::micro_unit_toolkit::data;

#pragma mark Utility

#define ALLOCATE_BSONT(x) ChaosBsonShrdPtr(x, &bsonDeallocator)

#define ACCESS_BSON(x) static_cast<bson_t*>(x.get())

#define ENSURE_ARRAY(x) \
if(x.get() == NULL) {array_index = 0; x = ALLOCATE_BSONT(bson_new());}

#define FIND_AND_CHECK(k,c)\
bson_iter_t element_found;\
bson_iter_init(&element_found, ACCESS_BSON(bson));\
if(bson_iter_find_case(&element_found, key.c_str()) && c(&element_found))

static void bsonDeallocator(bson_t* bson) {if(bson){bson_destroy(bson);}}

#pragma mark DataPack
DataPack::DataPack():
bson(ALLOCATE_BSONT(bson_new())),
array_index(0){
    assert(bson);
}

DataPack::DataPack(const bson_t *copy_bson):
bson(ALLOCATE_BSONT(bson_copy(copy_bson))),
array_index(0){}

DataPack::DataPack(const char* mem_ser,
                           uint32_t mem_size):
bson(ALLOCATE_BSONT(bson_new_from_data((const uint8_t*)mem_ser,
                                       mem_size))),
array_index(0){
    assert(bson);
}

DataPack::DataPack(const char* mem_ser):
array_index(0) {
    uint32_t size = BSON_UINT32_FROM_LE(*reinterpret_cast<const uint32_t *>(mem_ser));
    bson = ALLOCATE_BSONT(bson_new_from_data((const uint8_t*)mem_ser,
                                             size));
    assert(bson);
}

DataPack::DataPack(const std::string& json_document):
array_index(0) {
    bson_error_t err;
    bson = ALLOCATE_BSONT(bson_new_from_json((const uint8_t*)json_document.c_str(),
                                             json_document.size(),
                                             &err));
    assert(bson);
}

DataPack::~DataPack(){}

CDWUniquePtr DataPack::instanceFromJson(const std::string& json_serialization) {
    return CDWUniquePtr(new DataPack(json_serialization));
}

DataPack *DataPack::clone() {
    return new DataPack(bson.get());
}


//add a csdata value
void DataPack::addCDWValue(const std::string& key,
                               const DataPack& value) {
    bson_append_document(ACCESS_BSON(bson),
                         key.c_str(),
                         (int)key.size(),
                         ACCESS_BSON(value.bson));
}

//add a string value
void DataPack::addStringValue(const std::string& key, const std::string& value) {
    bson_append_utf8(ACCESS_BSON(bson),
                     key.c_str(),
                     (int)key.size(),
                     value.c_str(),
                     (int)value.size());
}

//append a strin gto an open array
void DataPack::appendStringToArray(const std::string& value) {
    ENSURE_ARRAY(bson_tmp_array);
    char str[12];
    sprintf(str, "%d", array_index++);
    bson_append_utf8(ACCESS_BSON(bson_tmp_array),
                     str,
                     -1,
                     value.c_str(),
                     (int)value.size());
}

//append a strin gto an open array
void DataPack::appendInt32ToArray(int32_t value) {
    ENSURE_ARRAY(bson_tmp_array);
    char str[12];
    sprintf(str, "%d", array_index++);
    bson_append_int32(ACCESS_BSON(bson_tmp_array),
                      str,
                      -1,
                      value);
}

//append a strin gto an open array
void DataPack::appendInt64ToArray(int64_t value) {
    ENSURE_ARRAY(bson_tmp_array);
    char str[12];
    sprintf(str, "%d", array_index++);
    bson_append_int64(ACCESS_BSON(bson_tmp_array),
                      str,
                      -1,
                      value);
}

//append a strin gto an open array
void DataPack::appendDoubleToArray(double value) {
    ENSURE_ARRAY(bson_tmp_array);
    char str[12];
    sprintf(str, "%d", array_index++);
    bson_append_double(ACCESS_BSON(bson_tmp_array),
                       str,
                       -1,
                       value);
}

//appen a DataPack to an open array
void DataPack::appendDataPackToArray(DataPack& value) {
    ENSURE_ARRAY(bson_tmp_array);
    char str[12];
    sprintf(str, "%d", array_index++);
    bson_append_document(ACCESS_BSON(bson_tmp_array),
                         str,
                         -1,
                         value.bson.get());
}

//finalize the array into a key for the current dataobject
void DataPack::finalizeArrayForKey(const std::string& key) {
    ENSURE_ARRAY(bson_tmp_array);
    bson_append_array(ACCESS_BSON(bson),
                      key.c_str(),
                      (int)key.size(),
                      bson_tmp_array.get());
    bson_tmp_array.reset();
}

//return a vectorvalue for a key
CMultiTypeDataArrayWrapper* DataPack::getVectorValue(const std::string& key)  const{
    return new CMultiTypeDataArrayWrapper(bson,
                                          key);
}

void DataPack::addNullValue(const std::string& key) {
    bson_append_null(ACCESS_BSON(bson),
                     key.c_str(),
                     (int)key.size());
}
//add a long value
void DataPack::addInt32Value(const std::string& key, int32_t value) {
    bson_append_int32(ACCESS_BSON(bson),
                      key.c_str(),
                      (int)key.size(),
                      value);
}
//add a long value
void DataPack::addInt32Value(const std::string& key, uint32_t value) {
    bson_append_int32(ACCESS_BSON(bson),
                      key.c_str(),
                      (int)key.size(),
                      static_cast<int32_t>(value));
}
//add a double value
void DataPack::addDoubleValue(const std::string& key, double value) {
    bson_append_double(ACCESS_BSON(bson),
                       key.c_str(),
                       (int)key.size(),
                       value);
}

//add a integer value
void DataPack::addInt64Value(const std::string& key, int64_t value) {
    bson_append_int64(ACCESS_BSON(bson),
                      key.c_str(),
                      (int)key.size(),
                      value);
}

//add a integer value
void DataPack::addInt64Value(const std::string& key, uint64_t value) {
    bson_append_int64(ACCESS_BSON(bson),
                      key.c_str(),
                      (int)key.size(),
                      static_cast<int64_t>(value));
}

CDWUniquePtr DataPack::getCDWValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_DOCUMENT){
        uint32_t document_len = 0;
        const uint8_t *document = NULL;
        bson_iter_document(&element_found,
                           &document_len,
                           &document);
        return CDWUniquePtr(new DataPack((const char *)document,document_len));
    } else {
        return CDWUniquePtr(new DataPack());
    }
}

//get string value
std::string  DataPack::getStringValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_UTF8){
        return std::string(bson_iter_utf8(&element_found, NULL));
    }
    return std::string();
}

//get string value
const char *  DataPack::getCStringValue(const std::string& key) const{
    return getRawValuePtr(key);
}

//add a integer value
int32_t DataPack::getInt32Value(const std::string& key,
                                    int32_t default_value) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_INT32){
        return bson_iter_int32(&element_found);
    } else {
        return default_value;
    }
}
//add a integer value
uint32_t DataPack::getUInt32Value(const std::string& key,
                                      uint32_t default_value) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_INT32){
        return static_cast<uint32_t>(bson_iter_int32(&element_found));
    } else {
        return default_value;
    }
}
//add a integer value
int64_t DataPack::getInt64Value(const std::string& key,
                                    int64_t default_value) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_INT64){
        return bson_iter_int64(&element_found);
    } else {
        return default_value;
    }
}
//add a integer value
uint64_t DataPack::getUInt64Value(const std::string& key,
                                      uint64_t default_value) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_INT64){
        return static_cast<uint64_t>(bson_iter_int64(&element_found));
    } else {
        return default_value;
    }
}
//add a integer value
double DataPack::getDoubleValue(const std::string& key,
                                    double default_value) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_DOUBLE){
        return bson_iter_double(&element_found);
    } else {
        return default_value;
    }
}

//get a bool value
bool  DataPack::getBoolValue(const std::string& key,
                                 bool default_value) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_BOOL){
        return bson_iter_bool(&element_found);
    } else {
        return default_value;
    }
}

//set a binary data value
void DataPack::addBinaryValue(const std::string& key,
                                  const char *buff,
                                  int bufLen) {
    bson_append_binary(ACCESS_BSON(bson),
                       key.c_str(),
                       (int)key.size(),
                       BSON_SUBTYPE_BINARY,
                       (const uint8_t *)buff,
                       bufLen);
}

//return the binary data value
const char* DataPack::getBinaryValue(const std::string& key, uint32_t& bufLen) const{
    const uint8_t* ret = NULL;
    unsigned int sub;
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_BINARY){
        bson_iter_binary(&element_found,
                         &sub,
                         &bufLen,
                         &ret);
    }
    return (const char*)ret;
}

//check if the key is present in data wrapper
bool DataPack::hasKey(const std::string& key) const{
    return bson_has_field(ACCESS_BSON(bson), key.c_str());
}

bool DataPack::isVector(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_ARRAY){
        return true;
    } else {
        return false;
    }
}

//return all key contained into the object
void DataPack::getAllKey(std::vector<std::string>& contained_key) const {
    bson_iter_t it;
    bson_iter_init(&it, ACCESS_BSON(bson));
    while(bson_iter_next(&it)) {
        contained_key.push_back(bson_iter_key(&it));
    }
}

//return all key contained into the object
void DataPack::getAllKey(std::set<std::string>& contained_key) const {
    bson_iter_t it;
    bson_iter_init(&it, ACCESS_BSON(bson));
    while(bson_iter_next(&it)) {
        contained_key.insert(bson_iter_key(&it));
    }
}

//return all key contained into the object
uint32_t DataPack::getValueSize(const std::string& key) const{
    bson_iter_t it;
    bson_iter_init(&it, ACCESS_BSON(bson));
    if(bson_iter_find_case(&it, key.c_str()) == false) return 0;
    const bson_value_t *v = bson_iter_value(&it);
    switch(v->value_type) {
        case BSON_TYPE_INT64:
            return sizeof(int64_t);
        case BSON_TYPE_INT32:
            return sizeof(int32_t);
        case BSON_TYPE_BOOL:
            return sizeof(bool);
        case BSON_TYPE_DOUBLE:
            return sizeof(double);
        case BSON_TYPE_UTF8:
            return v->value.v_utf8.len;
        case BSON_TYPE_BINARY:
            return v->value.v_binary.data_len;
        case BSON_TYPE_ARRAY:{
            uint32_t array_len = 0;
            const uint8_t *array = NULL;
            bson_iter_array(&it, &array_len, &array);
            return array_len;
        }
        case BSON_TYPE_DOCUMENT:{
            return v->value.v_doc.data_len;
        }
        default:
            return 0;
            break;
    }
    return 0;
}

//! return the raw value ptr address
const char * DataPack::getRawValuePtr(const std::string& key) const{
    bson_iter_t it;
    bson_iter_init(&it, ACCESS_BSON(bson));
    if(bson_iter_find_case(&it, key.c_str()) == false) return 0;
    const bson_value_t *v = bson_iter_value(&it);
    switch(v->value_type) {
        case BSON_TYPE_INT64:
            return reinterpret_cast<const char*>(&v->value.v_int64);
        case BSON_TYPE_INT32:
            return reinterpret_cast<const char*>(&v->value.v_int32);
        case BSON_TYPE_BOOL:
            return reinterpret_cast<const char*>(&v->value.v_bool);
        case BSON_TYPE_DOUBLE:
            return reinterpret_cast<const char*>(&v->value.v_double);
        case BSON_TYPE_UTF8:
            return static_cast<const char*>(v->value.v_utf8.str);
        case BSON_TYPE_BINARY:
            return reinterpret_cast<const char*>(v->value.v_binary.data);
        case BSON_TYPE_ARRAY:{
            uint32_t array_len = 0;
            const uint8_t *array = NULL;
            bson_iter_array(&it, &array_len, &array);
            return reinterpret_cast<const char*>(array);
        }
        case BSON_TYPE_DOCUMENT:{
            return reinterpret_cast<const char*>(v->value.v_doc.data);
        }
        default:
            return NULL;
            break;
    }
    
}

void DataPack::addBoolValue(const std::string& key, bool value) {
    bson_append_bool(ACCESS_BSON(bson),
                     key.c_str(),
                     (int)key.size(),
                     value);
}

const char* DataPack::getBSONRawData() const{
    return reinterpret_cast<const char*>(bson_get_data(ACCESS_BSON(bson)));
}

const int DataPack::getBSONRawSize() const{
    return bson->len;
}

//return the json data
std::string DataPack::toString()  const{
    size_t str_size = 0;
    char * str_c = bson_as_extended_json(ACCESS_BSON(bson),
                                         &str_size);
    std::string result(str_c);
    bson_free(str_c);
    return result;
}
//reinitialize the object with bson data
void DataPack::setSerializedData(const char* bson_data) {
    bson_iter_t it;
    size_t len = (size_t)BSON_UINT32_FROM_LE(*reinterpret_cast<const uint32_t *>(bson_data));
    if(!bson_iter_init_from_data(&it,
                                 reinterpret_cast<const uint8_t *>(bson_data),
                                 len)) return;
    while(bson_iter_next(&it)){
        bson_append_value(ACCESS_BSON(bson),
                          bson_iter_key(&it),
                          -1,
                          bson_iter_value(&it));
    }
    
}

//reinitialize the object with bson data
void DataPack::setSerializedJsonData(const char* json_data) {
    bson_error_t err;
    size_t len = (size_t)strlen(json_data);
    bson =ALLOCATE_BSONT(bson_new_from_json((const uint8_t*)json_data,
                                            len,
                                            &err));
    assert(bson);
}

//append all elemento of an
void DataPack::appendAllElement(DataPack& src_cdw) {
    bson_iter_t it;
    bson_iter_init(&it, ACCESS_BSON(src_cdw.bson));
    while (bson_iter_next(&it)) {
        bson_append_value(ACCESS_BSON(bson),
                          bson_iter_key(&it),
                          -1,
                          bson_iter_value(&it));
    }
}

bool DataPack::copyKeyTo(const std::string& key_to_copy,
                             DataPack& destination) {
    return copyKeyToNewKey(key_to_copy,
                           key_to_copy,
                           destination);
}

//!copy a key(with value) from this instance to another DataPack witha new key
bool DataPack::copyKeyToNewKey(const std::string& key_to_copy,
                                   const std::string& new_key,
                                   DataPack& destination) {
    bson_iter_t it;
    bson_iter_init(&it, ACCESS_BSON(bson));
    if(bson_iter_find_case(&it, key_to_copy.c_str()) == false) return false;
    bson_append_value(ACCESS_BSON(destination.bson),
                      new_key.c_str(),
                      (int)new_key.size(),
                      bson_iter_value(&it));
    return true;
}

void DataPack::copyAllTo(DataPack& destination) {
    bson_iter_t it;
    bson_iter_init(&it, ACCESS_BSON(bson));
    while (bson_iter_next(&it)) {
        bson_append_value(ACCESS_BSON(destination.bson),
                          bson_iter_key(&it),
                          -1,
                          bson_iter_value(&it));
    }
}

//reset the datawrapper
void DataPack::reset() {
    bson_reinit(ACCESS_BSON(bson));
    bson_tmp_array.reset();
}

std::string DataPack::toHash() const{
    char ret[33];
    bson_md5_t m;
    uint8_t digest[16];
    bson_md5_init(&m);
    bson_md5_append(&m,
                    bson_get_data(ACCESS_BSON(bson)),
                    bson->len);
    bson_md5_finish(&m,
                    digest);
    memset(ret, 0, 33);
    for(int i = 0; i < 16; ++i) {
        sprintf(&ret[i*2], "%02x", (unsigned int)digest[i]);
    }
    return std::string(ret, 33);
}

//------------------------checking utility

bool DataPack::isNullValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_NULL){
        return true;
    }
    return false;
}

bool DataPack::isBoolValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_BOOL){
        return true;
    }
    return false;
}

bool DataPack::isInt32Value(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_INT32){
        return true;
    }
    return false;
}

bool DataPack::isInt64Value(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_INT64){
        return true;
    }
    return false;
}

bool DataPack::isDoubleValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_DOUBLE){
        return true;
    }
    return false;
}

bool DataPack::isStringValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_UTF8){
        return true;
    }
    return false;
}

bool DataPack::isBinaryValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_BINARY){
        return true;
    }
    return false;
}

bool DataPack::isCDWValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_DOCUMENT){
        return true;
    }
    return false;
}

bool DataPack::isVectorValue(const std::string& key) const{
    FIND_AND_CHECK(key, BSON_ITER_HOLDS_ARRAY){
        return true;
    }
    return false;
}

DataPackType DataPack::getValueType(const std::string& key) const{
    DataPackType result = DataPackTypeNoType;
    bson_iter_t it;
    bson_iter_init(&it, ACCESS_BSON(bson));
    if(bson_iter_find_case(&it, key.c_str()) == false) return result;
    switch(bson_iter_type(&it)) {
        case BSON_TYPE_ARRAY:
            result = DataPackTypeVector;
            break;
        case BSON_TYPE_DOCUMENT:
            result = DataPackTypeObject;
            break;
        case BSON_TYPE_BINARY:
            result = DataPackTypeBinary;
            break;
        case BSON_TYPE_UTF8:
            result = DataPackTypeString;
            break;
        case BSON_TYPE_DOUBLE:
            result = DataPackTypeDouble;
            break;
        case BSON_TYPE_INT32:
            result = DataPackTypeInt32;
            break;
        case BSON_TYPE_INT64:
            result = DataPackTypeInt64;
            break;
        case BSON_TYPE_BOOL:
            result = DataPackTypeBool;
            break;
        case BSON_TYPE_NULL:
            result = DataPackTypeNULL;
            break;
        default:
            break;
            
    }
    return result;
}

bool DataPack::isEmpty() const {
    return (bson_count_keys(ACCESS_BSON(bson)) == 0);
}

#pragma mark CMultiTypeDataArrayWrapper
CMultiTypeDataArrayWrapper::CMultiTypeDataArrayWrapper(const ChaosBsonShrdPtr& _document_shrd_ptr,
                                                       const std::string& key):
document_shrd_ptr(_document_shrd_ptr) {
    bson_iter_t element_found;
    bson_iter_init(&element_found, ACCESS_BSON(_document_shrd_ptr));
    if(bson_iter_find_case(&element_found, key.c_str())&&
       BSON_ITER_HOLDS_ARRAY(&element_found)) {
        uint32_t array_len;
        const uint8_t *array;
        bson_iter_array(&element_found,
                        &array_len,
                        &array);
        
        if (bson_init_static(&array_doc, array, array_len)) {
            bson_iter_t iter;
            if(bson_iter_init(&iter, &array_doc)) {
                while(bson_iter_next(&iter)) {
                    bson_value_t copy;
                    bson_value_copy(bson_iter_value(&iter), &copy);
                    values.push_back(copy);
                }
            }
        }
    }
}

CMultiTypeDataArrayWrapper::~CMultiTypeDataArrayWrapper() {
    for(VectorBsonValuesIterator it = values.begin(),
        end = values.end();
        it != end;
        it++) {
        bson_value_destroy(&(*it));
    }
}

std::string CMultiTypeDataArrayWrapper::getStringElementAtIndex(const int pos) const{
    assert(values[pos].value_type == BSON_TYPE_UTF8);
    return std::string(values[pos].value.v_utf8.str, values[pos].value.v_utf8.len);
}

double CMultiTypeDataArrayWrapper::getDoubleElementAtIndex(const int pos) const{
    assert(values[pos].value_type == BSON_TYPE_DOUBLE);
    return values[pos].value.v_double;
}
int32_t CMultiTypeDataArrayWrapper::getInt32ElementAtIndex(const int pos) const{
    return values[pos].value.v_int32;
}
int64_t CMultiTypeDataArrayWrapper::getint64ElementAtIndex(const int pos) const{
    assert(values[pos].value_type == BSON_TYPE_INT64);
    return values[pos].value.v_int64;
}

bool CMultiTypeDataArrayWrapper::isStringElementAtIndex(const int pos) const{
    return values[pos].value_type == BSON_TYPE_UTF8;
}

bool CMultiTypeDataArrayWrapper::isDoubleElementAtIndex(const int pos) const{
    return values[pos].value_type == BSON_TYPE_DOUBLE;
}

bool CMultiTypeDataArrayWrapper::isInt32ElementAtIndex(const int pos) const{
    return values[pos].value_type == BSON_TYPE_INT32;
}

bool CMultiTypeDataArrayWrapper::isInt64ElementAtIndex(const int pos) const{
    return values[pos].value_type == BSON_TYPE_INT64;
}

bool CMultiTypeDataArrayWrapper::isDataPackElementAtIndex(const int pos) const{
    return values[pos].value_type == BSON_TYPE_DOCUMENT;
}

DataPack* CMultiTypeDataArrayWrapper::getDataPackElementAtIndex(const int pos) const{
    assert(values[pos].value_type == BSON_TYPE_DOCUMENT);
    return new DataPack((const char *)values[pos].value.v_doc.data, values[pos].value.v_doc.data_len);
}
size_t CMultiTypeDataArrayWrapper::size() const{
    return values.size();
}
