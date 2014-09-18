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

#define OPT_CU_ID           "device_id"
#define OPT_TIMEOUT         "timeout"
#define OPT_DST_TARGET      "dest_target"
#define OPT_DST_FILE        "dest_file"
#define OPT_DST_TYPE        "dest_type"

#define OPT_START_TIME      "start_time"
#define OPT_END_TIME		"end_time"


void printPercendDone(int percend_done) {
	std::cout << " " << setfill('0') << setw(4) << percend_done << "& " <<std::flush;
}

void printStep() {
	std::cout << "." <<std::flush;
}

int computePercent(uint64_t done, uint64_t all) {
	int result = ((double)done/(double)all)*100;
	return result;
}

int main(int argc, char* argv[])
{
	char buf[255];
	
	uint32_t timeout;
	string device_id;
	string dst_file;
	bool dest_target;
	bool dest_type;
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
		
		//! [UIToolkit Attribute Init]
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_CU_ID, "The identification string of the device", &device_id);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_TIMEOUT, "Timeout for wait the answer in milliseconds", 2000, &timeout);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<bool>(OPT_DST_TARGET, "Destination target file(true) or stdandard output(false)", true, &dest_target);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<bool>(OPT_DST_TYPE, "Destination date type binary(true) or string(false)", true, &dest_type);
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
		if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_DST_FILE)){
			if(!chaos::TimingUtil::dateWellFormat(start_time)) {
				throw CException(-2, "Invalid star date format", "check date");
			}
			start_ts = chaos::TimingUtil::getTimestampFromString(start_time);
		}
		
		if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_START_TIME)){
			if(!chaos::TimingUtil::dateWellFormat(end_time)) {
				throw CException(-3, "Invalid end date format", "check date");
			}
			
			end_ts = chaos::TimingUtil::getTimestampFromString(end_time);
		}
		
		//print the desttination target
		std::cout << "Set output target to"<<(dest_target?" file":" standard output") << std::endl;
		
		//get the timestamp for query boundary
		if(dest_target) {
			if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_DST_FILE)){
				std::cout << "Auto destination file generation:";
				getcwd(buf, 255);
				dst_file.assign(buf, strlen(buf));
				dst_file += "/"+device_id+".exp";
				std::cout << dst_file << std::endl;
			}
			std::cout << "Open destination file";
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
			
			destination_stream = &destination_file;
		} else {
			destination_stream = &std::cout;
			(*destination_stream) << "BEGIN EXPORT---------------------------------";
		}
		//we can allocate the channel
		std::cout << "Acquiring controller" << std::endl;
		DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(device_id, timeout);
		if(!controller) throw CException(4, "Error allcoating decive controller", "device controller creation");
		
		chaos::common::io::QueryFuture *query_future = NULL;
		std::cout << "Starting export" << std::endl;
		controller->executeTimeIntervallQuery(start_ts, end_ts, &query_future);
		if(query_future) {
			do {
				auto_ptr<CDataWrapper> q_result(query_future->getDataPack(true, timeout));
				if(q_result.get()) {
					retry = 0;
					//get serialization buffer by type
					auto_ptr<chaos::common::data::SerializationBuffer> ser(dest_type?q_result->getBSONData():q_result->getJSONData());
					
					//write data
					destination_stream->write(ser->getBufferPtr(), ser->getBufferLen());
				} else {
					break;
				}
				
				cicle_number++;
				if(!(cicle_number % 10)) {
					printStep();
				}
				if(!(cicle_number % 100)) {
					printPercendDone(computePercent(query_future->getCurrentElementIndex(), query_future->getTotalElementFound()));
				}
				
			} while((query_future->getCurrentElementIndex() < query_future->getTotalElementFound()) && retry < 3);
			//print last percent
			printPercendDone(computePercent(query_future->getCurrentElementIndex(), query_future->getTotalElementFound()));
			//release the query
			controller->releaseQuery(query_future);
		}
	} catch (CException& e) {
		std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    } catch (...) {
		std::cerr << "General error " << std::endl;
	}
	
    try {
        //! [UIToolkit Deinit]
        ChaosUIToolkit::getInstance()->deinit();
        //! [UIToolkit Deinit]
    } catch (CException& e) {
        std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    }
	if(!dest_target) {
		(*destination_stream) << "END EXPORT---------------------------------";
	}
	std::cout << std::endl << "Export done"<< std::endl;
	return 0;
}

