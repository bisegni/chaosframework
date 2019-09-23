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

#include "ObjectStorageTest.h"
#include "../ChaosMetadataService.h"

#include "ObjectStoreFindDeviceID.h"

using namespace chaos;
using namespace chaos::metadata_service::test;
using namespace chaos::metadata_service::object_storage;
ObjectStorageTest::ObjectStorageTest() {
    //initilize driver pool manager
    InizializableService::initImplementation(DriverPoolManager::getInstance(), NULL, "DriverPoolManager", __PRETTY_FUNCTION__);

    MapKVP &cutom_map_parameter = ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param;
    if(ChaosMetadataService::getInstance()->setting.object_storage_setting.test_find) {
        //enable test for find device id
        test_suite.push_back(ChaosMakeSharedPtr<ObjectStoreFindDeviceID>(cutom_map_parameter));
    }
}

ObjectStorageTest::~ObjectStorageTest() {
    //initilize driver pool manager
    InizializableService::deinitImplementation(DriverPoolManager::getInstance(), "DriverPoolManager", __PRETTY_FUNCTION__);
}


