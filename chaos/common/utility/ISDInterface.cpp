/*
 *	ISDInterface.cpp
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 26/08/12.
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

#include <chaos/common/global.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/ISDInterface.h>

#define ISD_LAPP LAPP_ << "[ISDInterface]- "


using namespace chaos;
using namespace chaos::utility;

/*!
 */
ISDInterface::ISDInterface() {
        //set the default value
    isdState = ISD_DEINTIATED;
}

/*!
 */
ISDInterface::~ISDInterface() {
    
}

/*!
 */
bool ISDInterface::initImplementation(ISDInterface *impl, CDataWrapper *initData, const char * const implName,  const char * const domainString)  {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        ISD_LAPP  << "Initializing " << implName;
        impl->isdState = ISD_INITING;
        impl->init(initData);
        impl->isdState = ISD_INITIATED;
        ISD_LAPP  << implName << "Initialized";
    } catch (CException ex) {
        ISD_LAPP  << "Error initializing " << implName;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return result;
}

/*!
 */
bool ISDInterface::deinitImplementation(ISDInterface *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        ISD_LAPP  << "Deinitializing " << implName;
        impl->isdState = ISD_DEINITING;
        impl->deinit();
        impl->isdState = ISD_DEINTIATED;
        ISD_LAPP  << implName << "Deinitialized";
    } catch (CException ex) {
        ISD_LAPP  << "Error Deinitializing " << implName;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return result;
}

/*!
 */
bool ISDInterface::startImplementation(ISDInterface *impl, const char * const implName,  const char * const domainString) {
    bool result = true;
    try {
        if(impl == NULL) throw CException(0, "Implementation is null", domainString);
        ISD_LAPP  << "Starting " << implName;
        impl->start();
        ISD_LAPP  << implName << "Started";
    } catch (CException ex) {
        ISD_LAPP  << "Error Starting " << implName;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return result;
}