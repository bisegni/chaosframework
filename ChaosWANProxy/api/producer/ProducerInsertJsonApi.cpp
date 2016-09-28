/*
 *	ProducerInsertJsonApi.cpp
 *	!CHAOS
 *	Created by Andrea Michelotti.
 *
 *    	Copyrigh 2016 INFN, National Institute of Nuclear Physics
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
#include "ProducerInsertJsonApi.h"

#include <cstring>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/ChaosCommon.h>
#include <boost/algorithm/string.hpp>

using namespace chaos::common::data;
using namespace chaos::wan_proxy::api::producer;

#define PRODUCER_INSERT_ERR(where, err, msg)\
MAKE_API_ERR(where, "producer_insert_err", err, "producer_insert_err_msg", msg)

#define PID_LAPP LAPP_ << "[ProducerInsertJsonApi] - "
#define PID_LDBG LDBG_ << "[ProducerInsertJsonApi] - "
#define PID_LERR LERR_ << "[ProducerInsertJsonApi] - " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "
static boost::posix_time::ptime const time_epoch(boost::gregorian::date(1970, 1, 1));
//! default constructor
ProducerInsertJsonApi::ProducerInsertJsonApi(persistence::AbstractPersistenceDriver *_persistence_driver):
AbstractApi("jsoninsert",
            _persistence_driver){
    
}

//! default destructor
ProducerInsertJsonApi::~ProducerInsertJsonApi() {
    
}

//! execute the api
int ProducerInsertJsonApi::execute(std::vector<std::string>& api_tokens,
                                   const Json::Value& input_data,
                                   std::map<std::string, std::string>& output_header,
                                   Json::Value& output_data) {
    CHAOS_ASSERT(persistence_driver)
    int err = 0;
    std::string err_msg;
    std::string producer_name;
    int cnt;
    uint64_t ts;
    if(api_tokens.size() == 0) {
        err_msg = "no producer name in the uri";
        PID_LERR << err_msg;
        PRODUCER_INSERT_ERR(output_data, -1, err_msg);
        return err;
    } /*else if(api_tokens.size() > 1) {
       err_msg = "too many param in the uri";
       PID_LERR << err_msg;
       
       PRODUCER_INSERT_ERR(output_data, -2, err_msg);
       return err;
       }*/
    for(cnt = 0;cnt<api_tokens.size();cnt++){
        
        if(cnt<api_tokens.size()-1){
            producer_name=producer_name + api_tokens[cnt] + "/";
        } else {
            producer_name=producer_name + api_tokens[cnt] ;
        }
    }
    
    
    //we nned to remove the timestamp because is the only one that need to be int64
    const Json::Value& dp_timestamp = const_cast<Json::Value&>(input_data).removeMember(chaos::DataPackCommonKey::DPCK_TIMESTAMP);
    
    if(dp_timestamp.isNull()) {

      
      ts = (boost::posix_time::microsec_clock::universal_time() - time_epoch).total_milliseconds();
      
       
        err_msg = "The timestamp is mandatory";
    } else if(!dp_timestamp.isInt64()) {
        err_msg = "The timestamp needs to be an int64 number," + input_data[chaos::DataPackCommonKey::DPCK_TIMESTAMP].asString();
        PID_LERR << err_msg;
        PRODUCER_INSERT_ERR(output_data, -4, err_msg);
        return err;
    } else {
        ts = dp_timestamp.asUInt64();
    }
    
    //we can proceed
    auto_ptr<CDataWrapper> output_dataset(new CDataWrapper());
    //	const std::string& producer_name = api_tokens[0];
    const std::string& producer_key = producer_name+"_o";
    
    // add the node unique id
    output_dataset->addStringValue(chaos::DataPackCommonKey::DPCK_DEVICE_ID, producer_name);
    // add timestamp of the datapack
    output_dataset->addInt64Value(chaos::DataPackCommonKey::DPCK_TIMESTAMP, ts);
    
    //scan other memebrs to create the datapack
    Json::Value::Members members = input_data.getMemberNames();
    for(Json::Value::Members::iterator it = members.begin();
        it != members.end();
        it++) {
        //get current value
        const Json::Value& dataset_element = input_data[*it];
        if(dataset_element.isArray()){
	  int sub_type;
            int type_size=4;
            int size=dataset_element.size();
            if(size>0){
                
                const Json::Value&value=dataset_element[0];
                
                if(value.isBool()){
                    //default is double
                    type_size=sizeof(bool);
                    sub_type = chaos::DataType::SUB_TYPE_BOOLEAN;
                } else if(value.isInt()){
                    type_size=sizeof(int32_t);
                    sub_type = chaos::DataType::SUB_TYPE_INT32;
                    
                } else if(value.isInt64()){
                    type_size=sizeof(int64_t);
                    sub_type = chaos::DataType::SUB_TYPE_INT64;
                } else if(value.isString()){
                    type_size=1024;
                    sub_type = chaos::DataType::SUB_TYPE_STRING;
                    
                }else {
                    type_size=sizeof(double);
                    sub_type = chaos::DataType::SUB_TYPE_DOUBLE;
                }
                char bytearray[type_size*size];
                for(int cnt=0;cnt<size;cnt++){
                    const Json::Value&value=dataset_element[cnt];
                    switch(sub_type){
                        case chaos::DataType::SUB_TYPE_DOUBLE:{
                            double val=value.asDouble();
                            output_dataset->appendDoubleToArray(val);
                            double *ptr=(double*)bytearray;
                            ptr[cnt]=val;
                            break;
                        }
                        case chaos::DataType::SUB_TYPE_INT32:{
                            int32_t val=value.asInt();
                            int32_t *ptr=(int32_t *)bytearray;
                            ptr[cnt]=val;
                            break;
                        }
                        case chaos::DataType::SUB_TYPE_UNSIGNED:{
                            uint32_t val=value.asUInt();
                            uint32_t *ptr=(uint32_t *)bytearray;
                            ptr[cnt]=val;
                            break;
                        }
                        default:{
                            int8_t val=value.asInt()&0xff;
                            int8_t *ptr=(int8_t *)bytearray;
                            ptr[cnt]=val;
                            break;
                        }
                        case chaos::DataType::SUB_TYPE_INT16:{
                            int16_t val=value.asInt()&0xffff;
                            int16_t *ptr=(int16_t *)bytearray;
                            ptr[cnt]=val;
                            break;
                        }

                        case chaos::DataType::SUB_TYPE_INT64:{
                            int64_t val=value.asInt64();
                            int64_t *ptr=(int64_t *)bytearray;
                            ptr[cnt]=val;
                            break;
                        }
                        case chaos::DataType::SUB_TYPE_BOOLEAN:{
                            bool val=value.asBool();
                            bool *ptr=(bool *)bytearray;
                            ptr[cnt]=val;
                            break;
                        }
                        case chaos::DataType::SUB_TYPE_STRING:{
                            std::string val=value.asString();
                            strncpy(&bytearray[cnt],val.c_str(),1024);
                            break;
                        }
                    }
                    
                }
                //output_dataset->finalizeArrayForKey(it);
                output_dataset->addBinaryValue(*it, bytearray, type_size*size);
                
            }
        } else {
	  if(dataset_element.isDouble()){
	    output_dataset->addDoubleValue(*it, dataset_element.asDouble());
	  } else if(dataset_element.isBool()){
                output_dataset->addBoolValue(*it, dataset_element.asBool());
	  } else if(dataset_element.isInt64()){
	    output_dataset->addInt64Value(*it, (int64_t)dataset_element.asInt64());
	  } else if(dataset_element.isInt()){
	    output_dataset->addInt32Value(*it, dataset_element.asInt());
	  } else if(dataset_element.isString()){
                output_dataset->addStringValue(*it, dataset_element.asString());
            }else {
                output_dataset->addDoubleValue(*it, dataset_element.asDouble());
                
            }
            
        }
    }
    
    //call persistence api for insert the data
    if((err = persistence_driver->pushNewDataset(producer_key,
                                                 output_dataset.get(),
                                                 2))) {
        err_msg = "Error during push of the dataset";
        PID_LERR << err_msg;
        PRODUCER_INSERT_ERR(output_data, err, err_msg);
        return err;
    }
    output_data["register_insert_err"] = 0;
    return err;
}
