/*
 *	DeviceApi.h
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

#include "DeviceApi.h"

using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::cnd;
using namespace chaos::cnd::api;

DeviceApi::DeviceApi() {
    DeclareAction::addActionDescritionInstance<DeviceApi>(this,
                                                          &DeviceApi::registerDevice,
                                                          "device",
                                                          "register_device",
                                                          "Start the registartion phae of a device");
}

DeviceApi::~DeviceApi() {
    
}

/*!
 Register a device
 */
CDataWrapper* DeviceApi::registerDevice(CDataWrapper *registrationdData, bool& detach) throw(CException) {
    if(registrationdData == NULL) throw CException(-1, "The package can't be null", __PRETTY_FUNCTION__);
    
    //try to register new device if noone has been
    return NULL;
}