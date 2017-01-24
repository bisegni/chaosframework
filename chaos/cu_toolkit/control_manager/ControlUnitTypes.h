/*
 *	ControlUnitTypes.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/01/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EA9E644A_0507_48F7_98FC_848E90BB8EE0_ControlUnitTypes_h
#define __CHAOSFramework_EA9E644A_0507_48F7_98FC_848E90BB8EE0_ControlUnitTypes_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/alarm/AlarmCatalog.h>

namespace  chaos {
    namespace cu {
        namespace control_manager {
            
            typedef enum {
                INIT_RPC_PHASE_CALL_INIT_STATE = 0,
                INIT_RPC_PHASE_INIT_SHARED_CACHE,
                INIT_RPC_PHASE_COMPLETE_OUTPUT_ATTRIBUTE,
                INIT_RPC_PHASE_COMPLETE_INPUT_ATTRIBUTE,
                INIT_RPC_PHASE_INIT_SYSTEM_CACHE,
                INIT_RPC_PHASE_CALL_UNIT_DEFINE_ATTRIBUTE,
                INIT_RPC_PHASE_CREATE_FAST_ACCESS_CASCHE_VECTOR,
                INIT_RPC_PHASE_CALL_UNIT_INIT,
                INIT_RPC_PHASE_UPDATE_CONFIGURATION,
                INIT_RPC_PHASE_PUSH_DATASET
            } InitRPCPhase;
            
            typedef enum {
                INIT_SM_PHASE_INIT_DB = 0,
                INIT_SM_PHASE_CREATE_DATA_STORAGE,
            } InitSMPhase;
            
            
            typedef enum {
                START_RPC_PHASE_UNIT = 0,
                START_RPC_PHASE_IMPLEMENTATION
            } StartRPCPhase;
            
            typedef enum {
                START_SM_PHASE_STAT_TIMER = 0
            } StartSMPhase;
            
            typedef enum {
                StateVariableTypeAlarmCU,
                StateVariableTypeAlarmDEV
            } StateVariableType;
            
            static const char * const StateVariableTypeAlarmCULabel = "cu_alarm";
            static const char * const StateVariableTypeAlarmDEVLabel   = "device_alarm";
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(boost::shared_ptr<chaos::common::data::CDataWrapper>, ACUStartupCommandList);
            
            CHAOS_DEFINE_MAP_FOR_TYPE(StateVariableType, chaos::common::alarm::AlarmCatalog, MapStateVariable);
        }
    }
}

#endif /* __CHAOSFramework_EA9E644A_0507_48F7_98FC_848E90BB8EE0_ControlUnitTypes_h */
