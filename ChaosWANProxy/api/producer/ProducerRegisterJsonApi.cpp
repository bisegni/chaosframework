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
#include "ProducerRegisterJsonApi.h"
#include "ProducerInsertJsonApi.h"
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
static boost::posix_time::ptime const time_epoch(boost::gregorian::date(1970, 1, 1));

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
	try{
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
	
	CDataWrapper cu_dataset,mds_registration_pack;
	CDataWrapper dataset_pack;
	Json::StyledWriter				json_writer;

    std::string json_string=json_writer.write(input_data);
    const char* json_string_p=json_string.c_str();
    PRA_LDBG << "TO DECODE:"<<json_string_p<<" string size:"<<json_string.size();
    dataset_pack.setSerializedJsonData(json_string_p);

	PRA_LDBG << dataset_pack.getJSONString();
   //int64_t ts = (boost::posix_time::microsec_clock::universal_time() - time_epoch).total_milliseconds();
    ChaosStringVector all_template_key;
    dataset_pack.getAllKey(all_template_key);

    for(ChaosStringVectorIterator it = all_template_key.begin();
           it != all_template_key.end();
           it++) {
           if(dataset_pack.isNullValue(*it)) {
        	   PRA_LERR << "Removed from template null value key:" << *it;
               continue;
           }
           CDataWrapper ds;
           int32_t dstype=0,subtype=0;
           int32_t size=0;
           ds.addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,*it);

           size =dataset_pack.getValueSize(*it);
           if(dataset_pack.isDoubleValue(*it)){
        	   dstype |= chaos::DataType::TYPE_DOUBLE;
        	   subtype= chaos::DataType::SUB_TYPE_DOUBLE;
           } else if(dataset_pack.isInt64Value(*it)){
        	   dstype |=  chaos::DataType::TYPE_INT64;
        	   subtype= chaos::DataType::SUB_TYPE_INT64;

           } else if(dataset_pack.isInt32Value(*it)){
        	   dstype |=  chaos::DataType::TYPE_INT32;
        	   subtype= chaos::DataType::SUB_TYPE_INT32;

           } else if(dataset_pack.isStringValue(*it)){
        	   dstype |=  chaos::DataType::TYPE_STRING;
        	   subtype= chaos::DataType::SUB_TYPE_STRING;

           } else if(dataset_pack.isBinaryValue(*it)){
        	   dstype |=  chaos::DataType::TYPE_BYTEARRAY;
           } else {
        	   dstype |= chaos::DataType::TYPE_DOUBLE;
        	   subtype= chaos::DataType::SUB_TYPE_DOUBLE;

           }
           if(dataset_pack.isVector(*it)){
               //dstype = chaos::DataType::TYPE_ACCESS_ARRAY;
               dstype = chaos::DataType::TYPE_BYTEARRAY;
           }
           ds.addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE,subtype);
           ds.addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,dstype);
           ds.addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION,chaos::DataType::Output);
           ds.addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE,size);

           PRA_LDBG<<"- ATTRIBUTE \""<<*it<<"\" SIZE:"<<size<<" TYPE:"<<dstype<<" SUBTYPE:"<<subtype;
		   cu_dataset.appendCDataWrapperToArray(ds);

       }
	cu_dataset.addInt64Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP,chaos::common::utility::TimingUtil::getTimeStamp());

	//close array for all device description
	cu_dataset.finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
	cu_dataset.addInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID,(int64_t)0);

    mds_registration_pack.addCSDataValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, cu_dataset);
    mds_registration_pack.addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_CONTROL_UNIT);
	DEBUG_CODE(PRA_LDBG << mds_registration_pack.getJSONString());
	
	if((err = persistence_driver->registerDataset(producer_name,
												  mds_registration_pack))) {
		err_msg = "Error in the dataset registration";
        PRA_LERR << err_msg<<" ret:"<<err;
		PRODUCER_REGISTER_ERR(output_data, -19, err_msg);
		return err;
	}else {
		output_data["producer_register_err"] = 0;
		output_data["producer_register_bson"] =  mds_registration_pack.getJSONString();
	}
	} catch(std::exception e){
		PRA_LERR << "Exception:"<<e.what();
		err=-1;
	} catch(...){
		PRA_LERR << "Exception during process";
		err=-2;
	}
	return err;
}
