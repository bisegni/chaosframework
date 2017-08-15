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

#ifndef __CHAOSFramework_EA9E644A_0507_48F7_98FC_848E90BB8EE0_ControlUnitTypes_h
#define __CHAOSFramework_EA9E644A_0507_48F7_98FC_848E90BB8EE0_ControlUnitTypes_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/alarm/AlarmCatalog.h>

namespace  chaos {
    namespace cu {
        namespace control_manager {
            //!forward declaration
            class ControlUnitApiInterface;
            
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
            
            CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<chaos::common::data::CDataWrapper>, ACUStartupCommandList);
            
            CHAOS_DEFINE_MAP_FOR_TYPE(StateVariableType, chaos::common::alarm::AlarmCatalog, MapStateVariable);
            
            typedef enum {
                ControlUnitProxyEventDefine,
                ControlUnitProxyEventInit,
                ControlUnitProxyEventStart,
                ControlUnitProxyEventStop,
                ControlUnitProxyEventDeinit,
            } ControlUnitProxyEvent;
            
            typedef boost::function<bool(const std::string&,//control unit id
            const std::string&,//attribute name
            const chaos::common::data::CDataVariant&)> AttributeHandlerFunctor; //value
            
            typedef boost::function<void(const std::string&,//control unit id
            const ControlUnitProxyEvent)> EventHandlerFunctor;//event type
            
            typedef boost::function<bool(const bool, //true load/false unload
            const std::string&,//control unit id
            const ChaosSharedPtr<ControlUnitApiInterface>& )> ProxyLoadHandler;//event type

        }
    }
}

#endif /* __CHAOSFramework_EA9E644A_0507_48F7_98FC_848E90BB8EE0_ControlUnitTypes_h */
