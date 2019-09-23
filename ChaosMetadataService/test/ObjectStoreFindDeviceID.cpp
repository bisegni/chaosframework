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

#include "ObjectStoreFindDeviceID.h"
#include "../DriverPoolManager.h"
#include <iostream>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::common::utility;
using namespace chaos::metadata_service::test;
using namespace chaos::metadata_service::object_storage;
using namespace chaos::metadata_service::object_storage::abstraction;

ObjectStoreFindDeviceID::ObjectStoreFindDeviceID(const MapKVP& _map_param):
map_param(_map_param){}

ObjectStoreFindDeviceID::~ObjectStoreFindDeviceID() {}

void ObjectStoreFindDeviceID::executeTest() {
    VectorObject found_object_page;
    std::string device_id = map_param.find("device-id")->second;
    uint64_t start_ts = 0;
    uint64_t end_ts = 0;
    if(!common::utility::TimingUtil::dateWellFormat(map_param.find("start-time")->second)) {
        throw CException(-2, "Invalid star date format", "check date");
    }
    start_ts = common::utility::TimingUtil::getTimestampFromString(map_param.find("start-time")->second);
    
    if(!common::utility::TimingUtil::dateWellFormat(map_param.find("end-time")->second)) {
        throw CException(-2, "Invalid star date format", "check date");
    }
    end_ts = common::utility::TimingUtil::getTimestampFromString(map_param.find("end-time")->second);
    
    chaos::common::direct_io::channel::opcode_headers::SearchSequence search_last_seq;
    std::cout << "Test find:" << device_id << std::endl;
    ObjectStorageDataAccess *osda = DriverPoolManager::getInstance()->getObjectStorageDrv().getDataAccess<ObjectStorageDataAccess>();
    int err =  osda->findObject(device_id,ChaosStringSet(),start_ts,end_ts,30,found_object_page,search_last_seq);
    
}
