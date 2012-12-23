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
        
        auto_ptr<chaos::edb::EntityDB> testDB (new chaos::edb::SQLiteEntityDB());
        testDB->initDB("test", false);
        
        chaos::edb::EntityDB::KeyIdAndValue keyInfo;
        int32_t keyDevice = 0;
        int32_t keyDataset = 0;
        int32_t idDevice = 0;
        int32_t idDataset = 0;
        
        testDB->getIDForKey("device", keyDevice);
        testDB->getIDForKey("dataset", keyDataset);
        
        keyInfo.keyID = keyDevice;
        keyInfo.type = chaos::edb::EntityDB::STR_VALUE;
        strcpy(keyInfo.value.strValue, "device_1");
        
            //add new device
        testDB->getIDForEntity(keyInfo, idDevice);
        
        keyInfo.keyID = keyDataset;
        keyInfo.type = chaos::edb::EntityDB::STR_VALUE;
        strcpy(keyInfo.value.strValue, "device_1");
        //add new device
        testDB->getIDForEntity(keyInfo, idDataset);

        
        testDB->deinitDB();
        
    } catch (chaos::CException& e) {
        std::cout << "Error code  = " << e.errorCode << std::endl;
        std::cout << "Error message  = " << e.errorMessage.c_str() << std::endl;
        std::cout << "Error domain  = " << e.errorDomain.c_str() << std::endl;
    }
    return 0;
}

