    //
    //  main.cpp
    //  TestEntityDB
    //
    //  Created by Claudio Bisegni on 23/12/12.
    //  Copyright (c) 2012 Claudio Bisegni. All rights reserved.
    //

#include <iostream>
#include <chaos/common/data/entity_db/sqlite_impl/SQLiteEntityDB.h>
#include <chaos/common/data/entity_db/EntityDB.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

int main(int argc, const char * argv[])
{
    
        // insert code here...
    try {
        
        chaos::ArrayPointer<chaos::edb::KeyIdAndValue> properties;
        auto_ptr<chaos::edb::EntityDB> testDB (new chaos::edb::SQLiteEntityDB());
        testDB->initDB("test_one", true);
        
        chaos::edb::KeyIdAndValue deviceInfo;
        chaos::edb::KeyIdAndValue propertyInfo;
        uint32_t keyDevice = 0;
        uint32_t keyDataset = 0;

        
        testDB->getIDForKey("device", keyDevice);
        testDB->getIDForKey("dataset", keyDataset);
        
        deviceInfo.keyID = keyDevice;
        deviceInfo.type = chaos::edb::KEY_STR_VALUE;
        strcpy(deviceInfo.value.strValue, "device_1");
        
        chaos::entity::Entity *entityA = testDB->getNewEntityInstance(deviceInfo);
        if(entityA) {
            
            entityA->getAllProperty(properties);
            
            propertyInfo.keyID = keyDevice;
            propertyInfo.type = chaos::edb::KEY_NUM_VALUE;
            propertyInfo.value.numValue = 23;

            entityA->addProperty(propertyInfo);
        }
        
        strcpy(deviceInfo.value.strValue, "device_2");
        chaos::entity::Entity *entityB = testDB->getNewEntityInstance(deviceInfo);
        
        entityA->addChild(*entityB);
        
        testDB->deinitDB();
        
    } catch (chaos::CException& e) {
        std::cout << "Error code  = " << e.errorCode << std::endl;
        std::cout << "Error message  = " << e.errorMessage.c_str() << std::endl;
        std::cout << "Error domain  = " << e.errorDomain.c_str() << std::endl;
    }
    return 0;
}

