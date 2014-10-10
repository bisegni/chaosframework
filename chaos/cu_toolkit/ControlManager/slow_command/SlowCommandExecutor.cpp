/*
 *	SlowCommandExecutor.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include <vector>
#include <string>

#include <chaos/common/global.h>

#include <chaos/cu_toolkit/ControlManager/SCAbstractControlUnit.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandExecutor.h>



using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::common::batch_command;


#define LOG_HEAD_SBE "[SlowCommandExecutor-" << deviceSchemaDbPtr->getDeviceID() << "] "

#define SCELAPP_ LAPP_ << LOG_HEAD_SBE
#define SCELDBG_ LDBG_ << LOG_HEAD_SBE
#define SCELERR_ LERR_ << LOG_HEAD_SBE
using namespace chaos::common::data::cache;
using namespace chaos::common::batch_command;

SlowCommandExecutor::SlowCommandExecutor(std::string _executorID,
										 DatasetDB *_deviceSchemaDbPtr,
										 SCAbstractControlUnit *_control_unit_instance):
BatchCommandExecutor(_executorID),
deviceSchemaDbPtr(_deviceSchemaDbPtr),
attribute_cache(new AttributeSharedCacheWrapper(getAttributeSharedCache())),
control_unit_instance(_control_unit_instance),
ts_hb_cache(NULL),
last_ru_id_cache(NULL),
last_acq_ts_cache(NULL){}

SlowCommandExecutor::~SlowCommandExecutor(){}

// Initialize instance
void SlowCommandExecutor::init(void *initData) throw(chaos::CException) {
    std::vector<std::string> attribute_names;
    
    //initialize superclass
	BatchCommandExecutor::init(initData);
}


//! Install a command associated with a type
void SlowCommandExecutor::installCommand(string alias, chaos::common::utility::NestedObjectInstancer<SlowCommand, chaos_batch::BatchCommand> *instancer) {
    //call superclss method
    BatchCommandExecutor::installCommand(alias, instancer);
}


//! Check if the waithing command can be installed
BatchCommand *SlowCommandExecutor::instanceCommandInfo(std::string& commandAlias) {
	//install command into the batch command executor root class
    SlowCommand *result = (SlowCommand*) BatchCommandExecutor::instanceCommandInfo(commandAlias);
	
	//cusotmize slow command sublcass
    if(result) {
		//forward the pointers of the needed data
		result->driverAccessorsErogator = driverAccessorsErogator;
        result->deviceDatabasePtr = deviceSchemaDbPtr;
		result->attribute_cache = attribute_cache;
    }
    return result;
}
//overlodaed command event handler
void SlowCommandExecutor::handleCommandEvent(uint64_t command_seq,
											 BatchCommandEventType::BatchCommandEventType type,
											 void* type_value_ptr) {
	//let the base class handle the event
	BatchCommandExecutor::handleCommandEvent(command_seq, type, type_value_ptr);
	
	//now we need to update the system cache or out cache
}

//! general sandbox event handler
void SlowCommandExecutor::handleSandboxEvent(const std::string& sandbox_id,
											 BatchSandboxEventType::BatchSandboxEventType type,
											 void* type_value_ptr,
											 uint32_t type_value_size) {
	//let the base class handle the event
	BatchCommandExecutor::handleSandboxEvent(sandbox_id, type, type_value_ptr, type_value_size);

	if(!last_ru_id_cache) {
		last_ru_id_cache = getAttributeSharedCache()->getVariableValue(SharedCacheInterface::SVD_SYSTEM,
																	   DataPackSystemKey::DP_SYS_RUN_UNIT_ID);
		if(!last_ru_id_cache) {
			SCELERR_ << "Error getting cache slot for unit id";
			return;
		}
	}

	//set the last system event sandbox id
	last_ru_id_cache->setValue(sandbox_id.c_str(), (uint32_t)sandbox_id.size());
	
	switch(type) {
		case BatchSandboxEventType::EVT_HEART_BEAT: {
			if(type_value_size == sizeof(uint64_t)) {
				uint64_t *hb = static_cast<uint64_t*>(type_value_ptr);
				if(!ts_hb_cache) {
					ts_hb_cache =  getAttributeSharedCache()->getVariableValue(SharedCacheInterface::SVD_SYSTEM,
																			   DataPackSystemKey::DP_SYS_HEARTBEAT);
					if(!ts_hb_cache) {
						SCELERR_ << "Error getti cache slot for heartbeat timestamp";
						return;
					}
				}
				ts_hb_cache->setValue(hb, type_value_size);
				//notify to push data
				control_unit_instance->pushSystemDataset();
			}
			break;
		}
			
		case BatchSandboxEventType::EVT_RUN: {
			uint64_t *hb = static_cast<uint64_t*>(type_value_ptr);
			if(!last_acq_ts_cache) {
				last_acq_ts_cache =  getAttributeSharedCache()->getVariableValue(SharedCacheInterface::SVD_OUTPUT,
																				 DataPackKey::CS_CSV_TIME_STAMP);
				if(!last_acq_ts_cache) {
					SCELERR_ << "Error gettin cache slot for acquisition timestamp";
					return;
				}
			}
			last_acq_ts_cache->setValue(hb, type_value_size);
			//push output dataset
			control_unit_instance->pushOutputDataset();
		}
	}
	
	
}