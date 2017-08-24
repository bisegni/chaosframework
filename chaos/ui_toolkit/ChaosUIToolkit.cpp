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

#include <chaos/ui_toolkit/LowLevelApi/LLDataApi.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>

using namespace std;
using namespace chaos;
using namespace chaos::ui;

#define UI_LAPP_ LAPP_ << "[UIToolkit] -"
/*
 
 */
ChaosUIToolkit::ChaosUIToolkit(){
	clientInstanceUUID = common::utility::UUIDUtil::generateUUIDLite();
}

/*
 
 */
ChaosUIToolkit::~ChaosUIToolkit(){
    deinit();
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
void ChaosUIToolkit::init(void *init_data) throw(CException) {
    try{
        ChaosCommon<ChaosUIToolkit>::init(init_data);
        
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
    try{
		UI_LAPP_ << "Deinit HLDataApi";
		HLDataApi::getInstance()->deinit();
		UI_LAPP_ << "HLDataApi deinitilized";
	}catch(...) {}
	
    try{
		UI_LAPP_ << "Deinit LLRpcApi";
		LLRpcApi::getInstance()->deinit();
		UI_LAPP_ << "LLRpcApi Deinitialized";
		UI_LAPP_ << "Deinitialization terminated";
	}catch(...) {}
	
    try{
		//forward the deinitialization to the common sublayer
		ChaosCommon<ChaosUIToolkit>::deinit();
	}catch(...) {}

}
	

