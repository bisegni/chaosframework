//
//  ChaosUIToolkit.cpp
//  UIToolkit
//
//  Created by bisegni on 08/09/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "ChaosUIToolkit.h"
#include "LowLevelApi/LLDataApi.h"
#include "../common/utility/UUIDUtil.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::ui;

/*
 
 */
ChaosUIToolkit::ChaosUIToolkit(){
	clientInstanceUUID = UUIDUtil::generateUUIDLite();
}

/*
 
 */
ChaosUIToolkit::~ChaosUIToolkit(){
    
}

/*
 
 */
void ChaosUIToolkit::init(int argc, const char* argv[]) throw(CException) {
    try{
            //init common stage
        ChaosCommon<ChaosUIToolkit>::init(argc, argv);

            //init the caches
        globalDatasetCache = new DeviceDatasetCache();
        if(globalDatasetCache) globalDatasetCache->init();
        
            //ssign static cache
        LLDataApi::datasetCache = globalDatasetCache;
    
            //istanziate HLDataApi
        hlDataApi = new HLDataApi();
    
            //instantiate hlRpcApi
        hlRpcApi = new HLRpcApi();
    
            //istanziate HLInfrastructureApi
        hlInfrastructureApi = new HLInfrastructureApi();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
}

/*
 
 */
void ChaosUIToolkit::deinit() throw(CException) {
    
    if(globalDatasetCache) {
        globalDatasetCache->deinit();
        DELETE_OBJ_POINTER(globalDatasetCache)
    }
}

