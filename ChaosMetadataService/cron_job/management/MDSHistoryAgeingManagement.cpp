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

#include "MDSHistoryAgeingManagement.h"
#include "../../DriverPoolManager.h"

#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::common::utility;
using namespace chaos::metadata_service::cron_job;
using namespace chaos::metadata_service::persistence::data_access;
using namespace chaos::metadata_service::object_storage::abstraction;

MDSHistoryAgeingManagement::MDSHistoryAgeingManagement(chaos::common::data::CDataWrapper *param):
MDSCronJob(param){}

MDSHistoryAgeingManagement::~MDSHistoryAgeingManagement() {}

void MDSHistoryAgeingManagement::start() {
    last_sequence_found = 0;
    log("Start MDSHistoryAgeingManagement");
}

#define AGEING_TO_MS(x) (((int64_t)x)*1000)
bool MDSHistoryAgeingManagement::execute(const common::cronus_manager::MapKeyVariant& job_parameter) {
    int err = 0;
    bool need_another_step = false;
    std::string control_unit_found = "";
    //in seconds
    uint32_t control_unit_ageing_time = 0;
    uint64_t last_ageing_perform_time = 0;
    uint64_t now = TimingUtil::getTimeStamp();

    auto *obj_storage_da = DriverPoolManager::getInstance()->getObjectStorageDrv().getDataAccess<ObjectStorageDataAccess>();
    auto *metadata_cu_da = DriverPoolManager::getInstance()->getPersistenceDrv().getDataAccess<ControlUnitDataAccess>();
    CHAOS_ASSERT(obj_storage_da && metadata_cu_da);
    
    if((err = metadata_cu_da->reserveControlUnitForAgeingManagement(last_sequence_found,
                                                                    control_unit_found,
                                                                    control_unit_ageing_time,
                                                                    last_ageing_perform_time))){
        log(CHAOS_FORMAT("Error %1% reserving control unit for ageing management", %err));
    } else if(control_unit_found.size()){
        const std::string output_key    = control_unit_found + DataPackPrefixID::OUTPUT_DATASET_POSTFIX;
        const std::string input_key     = control_unit_found + DataPackPrefixID::INPUT_DATASET_POSTFIX;
        const std::string system_key    = control_unit_found + DataPackPrefixID::SYSTEM_DATASET_POSTFIX;
        const std::string custom_key    = control_unit_found + DataPackPrefixID::CUSTOM_DATASET_POSTFIX;
        const std::string health_key    = control_unit_found + NodeHealtDefinitionKey::HEALT_KEY_POSTFIX;
        const std::string dev_alarm_key    = control_unit_found + DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX;
        const std::string cu_alarm_key    = control_unit_found + DataPackPrefixID::CU_ALARM_DATASET_POSTFIX;
        uint64_t remove_until_ts = now - AGEING_TO_MS(control_unit_ageing_time);
        log(CHAOS_FORMAT("Processing ageing for control unit %1% removing all data before %2% [now:(ms)%3%-(ms ageing time)%4%]", %control_unit_found%TimingUtil::toString(remove_until_ts)%now%AGEING_TO_MS(control_unit_ageing_time)));
        try {
            log(CHAOS_FORMAT("Remove data for key %1%", %output_key));
            
            if((err = obj_storage_da->deleteObject(output_key,
                                                   0,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %output_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %input_key));
            if((err = obj_storage_da->deleteObject(input_key,
                                                   0,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %input_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %system_key));
            if((err = obj_storage_da->deleteObject(system_key,
                                                   0,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %system_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %custom_key));
            if((err = obj_storage_da->deleteObject(custom_key,
                                                   0,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %custom_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %health_key));
            if((err = obj_storage_da->deleteObject(health_key,
                                                   0,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %health_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %dev_alarm_key));
            if((err = obj_storage_da->deleteObject(dev_alarm_key,
                                                   0,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %dev_alarm_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove data for key %1%", %cu_alarm_key));
            if((err = obj_storage_da->deleteObject(cu_alarm_key,
                                                   0,
                                                   remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing key %1% for control unit %2% with error %3%", %cu_alarm_key%control_unit_found%err));
            }
            
            log(CHAOS_FORMAT("Remove log for cu %1%", %control_unit_found));
            if((err = getDataAccess<persistence::data_access::LoggingDataAccess>()->eraseLogBeforTS(control_unit_found,
                                                                                                    remove_until_ts))){
                log(CHAOS_FORMAT("Error erasing logging for control unit %1% with error %2%", %control_unit_found%err));
            }
        }catch(CException& ex){
            log(ex.what());
        }catch(...){
            log("Undeterminated error during ageing management");
        }
        getDataAccess<persistence::data_access::ControlUnitDataAccess>()->releaseControlUnitForAgeingManagement(control_unit_found, true);
        
        need_another_step = true;
    } else {
        log("Control unit with empty string");
    }
    return need_another_step;
}

void MDSHistoryAgeingManagement::end() {
    log("End MDSHistoryAgeingManagement");
}
