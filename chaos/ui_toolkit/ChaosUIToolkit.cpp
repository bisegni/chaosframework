//
//  ChaosUIToolkit.cpp
//  UIToolkit
//
//  Created by bisegni on 08/09/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLDataApi.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/common/global.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_generators.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::ui;

#define UI_LAPP_ LAPP_ << "[UIToolkit] "
/*
 
 */
ChaosUIToolkit::ChaosUIToolkit(){
	clientInstanceUUID = UUIDUtil::generateUUIDLite();
    
    //init the caches
    globalDatasetCache = new DeviceDatasetCache();
}

/*
 
 */
ChaosUIToolkit::~ChaosUIToolkit(){
    if(globalDatasetCache) delete(globalDatasetCache);
}

/*
 
 */
void ChaosUIToolkit::init(int argc, const char* argv[]) throw(CException) {
    try{
           
        UI_LAPP_ << "Init ChaosUIToolkit";
         //init common substrate
        UI_LAPP_ << "Init Common substrate";
        ChaosCommon<ChaosUIToolkit>::init(argc, argv);

        
        if(globalDatasetCache) {
            UI_LAPP_ << "Init shared process ";
            globalDatasetCache->init();
        }
        
            //ssign static cache
        LLDataApi::datasetCache = globalDatasetCache;
        UI_LAPP_ << "Init LLRpcApi";
        LLRpcApi::getInstance()->init();
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
    }
    
}

