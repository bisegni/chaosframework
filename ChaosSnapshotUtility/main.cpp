/*
 *	main.cpp
 *	!CHAOS
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

#define OPT_CU_ID					"device-id"
#define OPT_CDS_ADDRESS				"cds-address"
#define OPT_SNAP_NAME				"snapshot-name"
#define OPT_SNAPSHOT_OP				"op"
#define OPT_SNAPSHOT_DS_TYPE		"dataset_type"
#define OPT_SNAPSHOT_TIMEOUT		"timeout"
using namespace chaos;
using namespace chaos::ui;
using namespace chaos::common::data;

int main(int argc, char * argv[]) {
	int64_t err = 0;
	std::cout << "main " << std::endl;
	std::vector<std::string> device_id_list;
	std::string snap_name;
	std::string cds_addr;
	uint32_t ds_type;
	uint32_t timeout = 0;
	unsigned int operation = 0;
	try{
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<std::string>(OPT_CDS_ADDRESS, "CDS address", &cds_addr);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_CU_ID, "The identification string of the device to snapshuot", &device_id_list);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<std::string>(OPT_SNAP_NAME, "The name of the snapshot", &snap_name);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<unsigned int>(OPT_SNAPSHOT_OP, "Operation on snapshot [create(0), delete(1), get(2), restore device(3)]", 0, &operation);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<unsigned int>(OPT_SNAPSHOT_DS_TYPE, "Dataset type [output(0), input(1), custom(2), system(3)]", 0, &ds_type);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_SNAPSHOT_TIMEOUT, "Timeout", 4000, &timeout);
		
		ChaosUIToolkit::getInstance()->init(argc, argv);
		//! [UIToolkit Init]
		
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CDS_ADDRESS)){
			throw CException(-1, "The cds address is mandatory", "check param");
		}
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SNAP_NAME)){
			throw CException(-2, "Invalid snapshot name set", "check param");
		}
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SNAPSHOT_OP)){
			throw CException(-3, "Invalid operation set", "check param");
		}
		if(!snap_name.size()) throw CException(-4, "Snapshot name can't zero-length", "check param");
		
		SystemApiChannel *system_api_channel = LLRpcApi::getInstance()->getSystemApiClientChannel(cds_addr);
		if(!system_api_channel) throw CException(-5, "Invalid system api channel instance", "getSystemApiClientChannel");
		
		chaos::common::direct_io::channel::opcode_headers::DirectIOSystemAPISnapshotResultPtr system_api_result = NULL;
		switch(operation) {
			case 0:{//new
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
				break;
			}
			case 1:{//delete
				if(!(err = system_api_channel->system_api_channel->deleteDatasetSnapshot(snap_name,
																						 &system_api_result))){
					if(system_api_result) {
						std::cout << "Snapshot delete report: " << std::endl;
						std::cout << "Error code:" << system_api_result->error << std::endl;
						std::cout << "Error message:" << system_api_result->error_message << std::endl;
						free(system_api_result);
					} else {
						std::cout << "no result received" << std::endl;
					}
				} else {
					std::cout << "Error executing directio call" << std::endl;
				}
				break;
			}
			case 2:{//get
				if(device_id_list.size()!=1) throw CException(-6, "For get oepration only a single producer key is required", "Get snapshot dataset operation");
				if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SNAPSHOT_DS_TYPE)){
					throw CException(-7, "The type of dataset is mandatory in get operation", "Get dataset from snapshot");
				}
				chaos::common::direct_io::channel::opcode_headers::DirectIOSystemAPIGetDatasetSnapshotResultPtr get_system_api_result = NULL;
				if(!(err = system_api_channel->system_api_channel->getDatasetSnapshotForProducerKey(snap_name,
																									device_id_list[0],
																									ds_type,
																									&get_system_api_result))){
					if(get_system_api_result) {
						std::cout << "Snapshot delete report: " << std::endl;
						std::cout << "Error code:" << get_system_api_result->api_result.error << std::endl;
						std::cout << "Error message:" << get_system_api_result->api_result.error_message << std::endl;
						if(get_system_api_result->channel_data) {
							auto_ptr<CDataWrapper> data(new CDataWrapper(((char*)get_system_api_result+sizeof(chaos::common::direct_io::channel::opcode_headers::DirectIOSystemAPISnapshotResult) + 4)));
							std::cout << "Data found-------------------------"<< std::endl;
							std::cout << data->getJSONString() << std::endl;
						}
						free(get_system_api_result);
					} else {
						std::cout << "no result received" << std::endl;
					}
				} else {
					std::cout << "Error executing directio call" << std::endl;
				}
				break;
			}
				
			case 3: {
				if(!device_id_list.size())  throw CException(-7, "Device id is mandatory for restore operation", "Restore a device to a tag");
				
				for (std::vector<std::string>::iterator it = device_id_list.begin();
					 it < device_id_list.end();
					 it++) {
					std::cout << "Try to restore the device " << *it << " to snapshot tag " << snap_name << std::endl;
					DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(*it, timeout);
					if(!controller) continue;
					
					//we can work
					controller->restoreDeviceToTag(snap_name);
					
					HLDataApi::getInstance()->disposeDeviceControllerPtr(controller);
				}
				
				break;
			}
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
