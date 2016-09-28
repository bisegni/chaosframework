/*
 *	ProducerRegisterAPI.h
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
#include "ProducerRegisterJsonApi.h"

#include <algorithm>
#include <string>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/ChaosCommon.h>
#include <boost/lexical_cast.hpp>

using namespace chaos::common::data;
using namespace chaos::wan_proxy::api::producer;

#define PRODUCER_REGISTER_ERR(where, err, msg)\
MAKE_API_ERR(where, "producer_register_err", err, "producer_register_err_msg", msg)

#define PRA_LAPP LAPP_ << "[ProducerRegisterJsonApi] - "
#define PRA_LDBG LDBG_ << "[ProducerRegisterJsonApi] - "
#define PRA_LERR LERR_ << "[ProducerRegisterJsonApi] - " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "

//! default constructor
ProducerRegisterJsonApi::ProducerRegisterJsonApi(persistence::AbstractPersistenceDriver *_persistence_driver):
AbstractApi("jsonregister",
			_persistence_driver){
	
}

//! default destructor
ProducerRegisterJsonApi::~ProducerRegisterJsonApi() {
	
}

//! execute the api
int ProducerRegisterJsonApi::execute(std::vector<std::string>& api_tokens,
										const Json::Value& input_data,
										std::map<std::string, std::string>& output_header,
										Json::Value& output_data) {
	CHAOS_ASSERT(persistence_driver)
	int err = 0;
	std::string err_msg;
	std::string producer_name;
	int cnt;
	if(api_tokens.size() == 0) {
		err_msg = "no producer name in the uri";
		PRA_LERR << err_msg;
		PRODUCER_REGISTER_ERR(output_data, -1, err_msg);
		return err;
	} 

	for(cnt = 0;cnt<api_tokens.size();cnt++){
	  
	  if(cnt<api_tokens.size()-1){
	    producer_name=producer_name + api_tokens[cnt] + "/";
	  } else {
	    producer_name=producer_name + api_tokens[cnt] ;
	  }
	}

    if(producer_name.empty()){
            err_msg = "empty producer name";
            PRA_LERR << err_msg;
            PRODUCER_REGISTER_ERR(output_data, -18, err_msg);
            return -18;

    }
	//we can proceed
	//const std::string& producer_name = api_tokens[0];
	PRA_LDBG << "Start producer json registration with id " << producer_name;
	
	CDataWrapper mds_registration_pack;
	CDataWrapper dataset_pack;
	
    boost::posix_time::ptime pt=boost::posix_time::microsec_clock::local_time();
    dataset_pack.addInt64Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP,
                                        (int64_t)pt.time_of_day().total_milliseconds());
    
  
    
		//we have a dataset, perhaps empty...
    Json::Value::Members members=input_data.getMemberNames();
 for(Json::Value::Members::iterator i =members.begin();i!=members.end();i++){

		boost::shared_ptr<CDataWrapper> element;
		if((err = scanDatasetElement(input_data[*i],*i, err_msg, element))) {
			PRA_LERR << err_msg;
			
			PRODUCER_REGISTER_ERR(output_data, err, err_msg);
			return err;
		} else {
			// add parametere representation object to main action representation
			dataset_pack.appendCDataWrapperToArray(*element.get());
		}
	}
	
	//close array for all device description
	dataset_pack.finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    
    mds_registration_pack.addCSDataValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, dataset_pack);
    mds_registration_pack.addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_CONTROL_UNIT);
	DEBUG_CODE(PRA_LDBG << mds_registration_pack.getJSONString());
	
	if((err = persistence_driver->registerDataset(producer_name,
												  mds_registration_pack))) {
		err_msg = "Error in the dataset registration";
		PRA_LERR << err_msg;
		PRODUCER_REGISTER_ERR(output_data, -19, err_msg);
		return err;
	}else {
		output_data["producer_register_err"] = 0;
		output_data["producer_register_bson"] =  mds_registration_pack.getJSONString();
	}
	return err;
}

//scan a json elemenot of the dataset creating the respective CDataWrapper
int ProducerRegisterJsonApi::scanDatasetElement(const Json::Value& dataset_json_element,std::string &name,
												   std::string& err_msg,
												   boost::shared_ptr<chaos::common::data::CDataWrapper>& element) {
	element.reset(new CDataWrapper());
	/*-the name of the attribute
	 "ds_attr_name": string,
	 */
    std::string attribute_name =name;
	if(attribute_name.empty()) {
		err_msg = "no attribute name found";
		return -6;
	}
	
	element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
							attribute_name);
    PRA_LDBG<<"==== attribute "<<attribute_name;
    
   	element->addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION,
							attribute_name);
    
    if(dataset_json_element.isArray()){
        element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,
                               chaos::DataType::TYPE_BYTEARRAY);
        int32_t size=dataset_json_element.size();
        if(size>0){
            // we can find the type
            
            const Json::Value& value=dataset_json_element[0];
            element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY,
                                    size);
            if(value.isBool()){
                //default is double
                element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE,
                                       chaos::DataType::SUB_TYPE_BOOLEAN);
                PRA_LDBG<<"Boolean array ["<<size<<"]";
            } else if(value.isInt()){
                //default is double
                element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE,
                                       chaos::DataType::SUB_TYPE_INT32);
                PRA_LDBG<<"Int32 array ["<<size<<"]";

            } else if(value.isInt64()){
                //default is double
                element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE,
                                       chaos::DataType::SUB_TYPE_INT64);
            } else if(value.isString()){
                //default is double
                element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE,
                                       chaos::DataType::SUB_TYPE_STRING);
                PRA_LDBG<<"String array ["<<size<<"]";

            }else {
                //default is double
                element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE,
                                       chaos::DataType::SUB_TYPE_DOUBLE);
                PRA_LDBG<<"Double array ["<<size<<"]";

            }
        } else {
            // uknown type
            //default is double
            element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE,
                                   chaos::DataType::SUB_TYPE_DOUBLE);
            PRA_LDBG<<"Uknown array, setting default type to double";

        }
        
    } else {

      if(dataset_json_element.isDouble()){ 
            //default is double
            element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,
                                   chaos::DataType::TYPE_DOUBLE);
            PRA_LDBG<<"Double Type";

        } else if(dataset_json_element.isBool()){
            //default is double
            element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,
                                   chaos::DataType::TYPE_BOOLEAN);
            PRA_LDBG<<"Boolean Type";

        } else if(dataset_json_element.isInt64()){
            //default is double
            element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,
                                   chaos::DataType::TYPE_INT64);
        } else if(dataset_json_element.isInt()){
            //default is double
            element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,
                                   chaos::DataType::TYPE_INT32);
            PRA_LDBG<<"Int32 Type";

        }  else if(dataset_json_element.isString()){
            //default is double
            element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,
                                   chaos::DataType::TYPE_STRING);
            PRA_LDBG<<"String Type";

        } else {
            //default is double
            element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,
                                   chaos::DataType::TYPE_DOUBLE);
            PRA_LDBG<<"Double Type";

        }

    }
	
		
    element->addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION,
							   chaos::DataType::Output);
	
    return 0;
}
 
