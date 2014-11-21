/*
 *	cde.cpp
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

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>

#include <chaos/common/bson/util/base64.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>


#include <chaos/common/bson/bson.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace bson;
using namespace boost;

#define OPT_CU_ID           "device-id"
#define OPT_TIMEOUT         "timeout"
#define OPT_DST_TARGET      "dest-target"
#define OPT_DST_FILE        "dest-file"
#define OPT_DST_TYPE        "dest-type"

#define OPT_START_TIME      "start-time"
#define OPT_END_TIME		"end-time"


void sendBackOnRow() {
	std::cout << "\r" <<std::flush;
}

void sendBackForNPos(int position = 1) {
	for(int i = 0; i < position; i++) {
		std::cout << "\b";
	}
	std::cout << std::flush;
}

void printPercendDone(int percend_done) {
	std::cout << " " << setfill('0') << setw(3) << percend_done << "% " <<std::flush;
}

void printStat(chaos::common::io::QueryFuture *query_future) {
	std::cout << "--------------------------------------------------------------------------" << std::endl;
	std::cout << "Total found: " << query_future->getTotalElementFound() <<std::endl;
	std::cout << "Last element fetched: " << query_future->getCurrentElementIndex() <<std::endl;
	std::cout << "Error code: " << query_future->getError() <<std::endl;
	std::cout << "Error message: " << query_future->getErrorMessage() <<std::endl;
	std::cout << "--------------------------------------------------------------------------" << std::endl;
}

void printStep() {
	std::cout << "." <<std::flush;
}

int computePercent(uint64_t done, uint64_t all) {
	int result = ((double)done/(double)all)*100;
	return result;
}

chaos::common::data::SerializationBuffer *getCSVDecoding( DeviceController& controller, const std::vector<std::string>& output_element_name, CDataWrapper& data_pack) {
	chaos::common::data::SerializationBuffer *result = NULL;
	std::stringstream csv_lin;
	chaos::RangeValueInfo attribute_info;
	
	int idx = 0;
	for(std::vector<std::string>::const_iterator it = output_element_name.begin();
		it < output_element_name.end();
		it++){
		//fetch the type
		controller.getDeviceAttributeRangeValueInfo(*it, attribute_info);
		
		//write the data in csv way
		switch(attribute_info.valueType){
			case DataType::TYPE_BOOLEAN:{
				if(data_pack.hasKey((*it).c_str())){
					csv_lin << data_pack.getBoolValue((*it).c_str());
				}
				break;
			}
				
			case DataType::TYPE_BYTEARRAY:{
				int len;
				std::stringstream binary_field;
				const char * base_addr = data_pack.getBinaryValue((*it).c_str(), len);
				bson::base64::encode( binary_field , base_addr , len );
				csv_lin << binary_field.str();
				break;
			}
				
			case DataType::TYPE_DOUBLE:
				if(data_pack.hasKey((*it).c_str())){
					csv_lin << data_pack.getDoubleValue((*it).c_str());
				}
				break;
			case DataType::TYPE_INT32:{
				if(data_pack.hasKey((*it).c_str())){
					csv_lin << data_pack.getInt32Value((*it).c_str());
				}
				break;
			}
				
			case DataType::TYPE_INT64:{
				if(data_pack.hasKey((*it).c_str())){
					csv_lin << data_pack.getInt64Value((*it).c_str());
				}
				break;
			}
				
			case DataType::TYPE_STRING:{
				if(data_pack.hasKey((*it).c_str())){
					csv_lin << data_pack.getStringValue((*it).c_str());
				}
				break;
			}
				
			default:
				break;
		}
		if(++idx < output_element_name.size()) {
			csv_lin << ",";
		}else{
			csv_lin << std::endl;
		}
	}
	std::string line = csv_lin.str();
	if(line.size()>0) {
		result = new chaos::common::data::SerializationBuffer(line.c_str(), line.size());
	}
	return result;
}

int main(int argc, char* argv[])
{
	char buf[255];
	
	uint32_t timeout;
	string device_id;
	string dst_file;
	unsigned int dest_type;
	string start_time;
	string end_time;
	std::string err_str;
	std::ostream	*destination_stream = NULL;
	std::ofstream	destination_file;
	
	uint64_t start_ts = 0;
	uint64_t end_ts = 0;
	
	int retry = 0;
	uint32_t cicle_number = 0;
	CDeviceNetworkAddress deviceNetworkAddress;
	try{
		std::cout << "\e[?25l";
		//! [UIToolkit Attribute Init]
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_CU_ID, "The identification string of the device", &device_id);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_TIMEOUT, "Timeout for wait the answer in milliseconds", 2000, &timeout);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<unsigned int>(OPT_DST_TYPE, "Destination date type [binary(0), JSON(1), CSV(2)]", 0, &dest_type);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_DST_FILE, "Destination file for save found datapack", &dst_file);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_START_TIME, "Time for first datapack to find [format from %Y-%m-%dT%H:%M:%S.%f to %Y]", &start_time);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_END_TIME, "Time for last datapack to find [format from %Y-%m-%dT%H:%M:%S.%f to %Y]", &end_time);
		
		//! [UIToolkit Attribute Init]
		
		//! [UIToolkit Init]
		ChaosUIToolkit::getInstance()->init(argc, argv);
		//! [UIToolkit Init]
		
		
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CU_ID)){
			throw CException(-1, "invalid device identification string", "check param");
		}
		
		//get the timestamp for query boundary
		if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_START_TIME)){
			if(!chaos::TimingUtil::dateWellFormat(start_time)) {
				throw CException(-2, "Invalid star date format", "check date");
			}
			start_ts = chaos::TimingUtil::getTimestampFromString(start_time);
			std::cout << "Set start data to"<< start_time << std::endl;
		}
		
		if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_END_TIME)){
			if(!chaos::TimingUtil::dateWellFormat(end_time)) {
				throw CException(-3, "Invalid end date format", "check date");
			}
			
			end_ts = chaos::TimingUtil::getTimestampFromString(end_time);
			std::cout << "Set end data to"<< end_time << std::endl;
		}
		
		//get the timestamp for query boundary
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_DST_FILE)){
			std::cout << "Auto destination file generation" << std::endl;
			getcwd(buf, 255);
			dst_file.assign(buf, strlen(buf));
			dst_file += "/"+device_id;
			switch(dest_type) {
				case 0:
					dst_file.append(".bin");
					break;
				case 1:
					dst_file.append(".json");
					break;
				case 2:
					dst_file.append(".cvs");
					break;
			}
		}
		std::basic_ios<char>::openmode dst_file_mode = ios_base::out;
		if(dest_type) {
			dst_file_mode |= ios_base::binary;
		}
		destination_file.open(dst_file.c_str(), dst_file_mode);
		if(!destination_file.good()) {
			err_str = "Error opening destination file ";
			err_str.append(buf, strlen(buf));
			throw CException(-4, err_str, string("check param"));
		}
		std::cout << "Destination file -> " << dst_file << std::endl;
		destination_stream = &destination_file;
		
		//we can allocate the channel
		std::cout << "Acquiring controller" << std::endl;
		DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(device_id, timeout);
		if(!controller) throw CException(4, "Error allcoating decive controller", "device controller creation");
		
		
		chaos::common::io::QueryFuture *query_future = NULL;
		controller->executeTimeIntervallQuery(start_ts, end_ts, &query_future);
		
		std::vector<std::string> output_element_name;
		//fetche the output element of the device
		controller->getDeviceDatasetAttributesName(output_element_name, DataType::Output);

		//create header
		if(dest_type == 2) {
			
			//write header
			int idx = 0;
			for(std::vector<std::string>::const_iterator it = output_element_name.begin();
				it < output_element_name.end();
				it++){
				(*destination_stream) << *it;
				if(++idx < output_element_name.size()) {
					(*destination_stream) << ",";
				}
			}
		}
		
		if(query_future) {
			query_future->waitForBeginResult();
			while(query_future->getState() ==chaos::common::io::QueryFutureStateStartResult ||
				  query_future->getState() ==chaos::common::io::QueryFutureStateReceivingResult){
				auto_ptr<CDataWrapper> q_result(query_future->getDataPack(true, timeout));
				if(q_result.get()) {
					retry = 0;
					auto_ptr<chaos::common::data::SerializationBuffer> ser;
					//get serialization buffer by type
					switch (dest_type) {
							//BSON
						case 0:{
							ser.reset(q_result->getBSONData());
							break;
						}
							//JSON
						case 1:{
							ser.reset(q_result->getJSONData());
							break;
						}
							//CSV
						case 2:{
							ser.reset(getCSVDecoding(*controller, output_element_name, *q_result.get()));
							break;
						}
					}
					//write the data
					if(ser.get())destination_stream->write(ser->getBufferPtr(), ser->getBufferLen());
					
				} else {
					break;
				}
				
				cicle_number++;
				
				if(!(cicle_number % 10)) {
					std::cout << "Exporting ";
					printPercendDone(computePercent(query_future->getCurrentElementIndex(), query_future->getTotalElementFound()));
					sendBackOnRow();
				}
				
				//print last percent
				if(query_future->getState() != chaos::common::io::QueryFutureStateError) {
					std::cout << "Exporting ";
					printPercendDone(computePercent(query_future->getCurrentElementIndex(), query_future->getTotalElementFound()));
					std::cout << std::endl;
				}

			};
						//print the statistic
			printStat(query_future);
			std::cout << "Releasing query" << std::endl;
			//release the query
			controller->releaseQuery(query_future);
			std::cout << "Releasing controller" << std::endl;
			HLDataApi::getInstance()->disposeDeviceControllerPtr(controller);
		}
	} catch (CException& e) {
		std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
	} catch (...) {
		std::cerr << "General error " << std::endl;
	}
	std::cout << "\e[?25h";
	try {
		//! [UIToolkit Deinit]
		ChaosUIToolkit::getInstance()->deinit();
		//! [UIToolkit Deinit]
	} catch (CException& e) {
		std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
	}
	std::cout << std::endl << "Export done"<< std::endl;
	return 0;
}

