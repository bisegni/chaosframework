/*
 *	main.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#include <string>
#include <iostream>

#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>

#define OPT_CU_ID			"device-id"
#define OPT_CDS_ADDRESS	"cds-address"
#define OPT_SNAP_NAME		"snapshot-name"
#define OPT_SNAPSHOT_OP	"op"

using namespace chaos;
using namespace chaos::ui;

int main(int argc, char * argv[]) {
	int64_t err = 0;
	std::cout << "main " << std::endl;
	std::vector<std::string> device_id_list;
	std::string snap_name;
	std::string cds_addr;
	unsigned int operation;
	try{
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<std::string>(OPT_CDS_ADDRESS, "CDS address", &cds_addr);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CU_ID, "The identification string of the device to snapshuot", &device_id_list);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<std::string>(OPT_SNAP_NAME, "The name of the snapshot", &snap_name);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<unsigned int>(OPT_SNAPSHOT_OP, "Operation on snapshot [create(0), delete(1)]", 0, &operation);
		
		ChaosUIToolkit::getInstance()->init(argc, argv);
		//! [UIToolkit Init]
		
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CDS_ADDRESS)){
			throw CException(-1, "The cds address is mandatory", "check param");
		}
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SNAP_NAME)){
			throw CException(-2, "Invalid snapshot name set", "check param");
		}
		
		if(!snap_name.size()) throw CException(-3, "Snapshot name can't zero-length", "check param");
		
		SystemApiChannel *system_api_channel = LLRpcApi::getInstance()->getSystemApiClientChannel(cds_addr);
		
		chaos::common::direct_io::channel::opcode_headers::DirectIOSystemAPISnapshotResultPtr system_api_result = NULL;
		
		//!make snap on device
		if(!(err = system_api_channel->system_api_channel->makeNewDatasetSnapshot(snap_name,
																			device_id_list,
																			&system_api_result))){
			if(system_api_result) {
				std::cout << "Snapshot creation report: " << std::endl;
				std::cout << "Error code:" << system_api_result->error << std::endl;
				std::cout << "Error message:" << system_api_result->error_message << std::endl;
				free(system_api_result);
			} else {
				std::cout << "no result received" << std::endl;
			}
		} else {
			std::cout << "Error executing directio call" << std::endl;
		}
		
		if(system_api_channel) {
			LLRpcApi::getInstance()->releaseSystemApyChannel(system_api_channel);
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
	return 0;
}
