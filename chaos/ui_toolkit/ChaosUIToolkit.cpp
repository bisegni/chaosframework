/*	
 *	ChaosUIToolkit.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLDataApi.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>

using namespace std;
using namespace chaos;
using namespace chaos::ui;

#define UI_LAPP_ LAPP_ << "[UIToolkit] -"
/*
 
 */
ChaosUIToolkit::ChaosUIToolkit(){
	clientInstanceUUID = UUIDUtil::generateUUIDLite();
}

/*
 
 */
ChaosUIToolkit::~ChaosUIToolkit(){
}

    //! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosUIToolkit::init(int argc, char* argv[]) throw (CException) {
    ChaosCommon<ChaosUIToolkit>::init(argc, argv);
}
    //!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosUIToolkit::init(istringstream &initStringStream) throw (CException) {
   ChaosCommon<ChaosUIToolkit>::init(initStringStream);
}

/*
 
 */
void ChaosUIToolkit::init() throw(CException) {
    try{
        ChaosCommon<ChaosUIToolkit>::init();
        
        UI_LAPP_ << "Init LLRpcApi";
        LLRpcApi::getInstance()->init();
        UI_LAPP_ << "LLRpcApi Initilized";
        UI_LAPP_ << "Initialization terminated";
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
}

/*
 
 */
void ChaosUIToolkit::deinit() throw(CException) {
    
    UI_LAPP_ << "Deinit HLDataApi";
    HLDataApi::getInstance()->deinit();
    UI_LAPP_ << "HLDataApi deinitilized";
    
    UI_LAPP_ << "Deinit LLRpcApi";
    LLRpcApi::getInstance()->deinit();
    UI_LAPP_ << "LLRpcApi Deinitialized";
    UI_LAPP_ << "Deinitialization terminated";
}
