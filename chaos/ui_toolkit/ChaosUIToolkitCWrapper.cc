/*
 *	ChaosUIToolkit.h
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

#include "ChaosUIToolkitCWrapper.h"

#include <map>

#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace boost;


uint32_t sequenceNumber = 0;
std::map<uint32_t, DeviceController *> chanelMap;

DeviceController *getDeviceControllerFromID(uint32_t did) {
    if(chanelMap.count(did)==0) return NULL;
    return chanelMap[did];
}

char * convertStringToCharPtr(string& orgString) {
    char *result = NULL;
    if(!orgString.length()) return NULL;
    
    result = (char*)malloc(sizeof(char) * orgString.length());
    return result;
}

extern "C" {
    
    
    
    int initToolkit(const char* startupParameter) {
        int err = 0;
        istringstream optionStream;
        optionStream.str(startupParameter);
        try {
             ChaosUIToolkit::getInstance()->init(optionStream);
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int getNewControllerForDeviceID(const char* deviceID, uint32_t *devIDPtr) {
        int err = 0;
        DeviceController *controller = NULL;
        try{
            string dID = deviceID;
            controller = HLDataApi::getInstance()->getControllerForDeviceID(dID);
            if(controller) {
                chanelMap.insert(make_pair((*devIDPtr = ++sequenceNumber), controller));
                
                    //activate the traking
                controller->setupTracking();
            }
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int initDevice(uint32_t devID) {
        int err = 0;
        try{
            if(devID) {
                DeviceController * ctrl = getDeviceControllerFromID(devID);
                if(ctrl){
                    err = ctrl->initDevice();
                }else{
                    err = -1001;
                }
            } else {
                err = -1000;
            }
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int startDevice(uint32_t devID) {
        int err = 0;
        try{
            if(devID) {
                DeviceController * ctrl = getDeviceControllerFromID(devID);
                if(ctrl){
                    err = ctrl->startDevice();
                }else{
                    err = -1001;
                }
            } else {
                err = -1000;
            }
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int stopDevice(uint32_t devID) {
        int err = 0;
        try{
            if(devID) {
                DeviceController * ctrl = getDeviceControllerFromID(devID);
                if(ctrl){
                    err = ctrl->stopDevice();
                }else{
                    err = -1001;
                }
            } else {
                err = -1000;
            }
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int deinitDevice(uint32_t devID) {
        int err = 0;
        try{
            if(devID) {
                DeviceController * ctrl = getDeviceControllerFromID(devID);
                if(ctrl){
                    err = ctrl->deinitDevice();
                }else{
                    err = -1001;
                }
            } else {
                err = -1000;
            }
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int fetchLiveData(uint32_t devID) {
        int err = 0;
        try{
            if(devID) {
                DeviceController * ctrl = getDeviceControllerFromID(devID);
                if(ctrl){
                    ctrl->fetchCurrentDeviceValue();
                }else{
                    err = -1001;
                }
            } else {
                err = -1000;
            }
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int getStrValueForAttribute(uint32_t devID, const char * dsAttrName, char ** dsAttrValue) {
        int err = 0;
        string tmpString;
        try{
            DeviceController *dCtrl = getDeviceControllerFromID(devID);
            if(dCtrl && dsAttrName && dsAttrValue) {
                CDataWrapper *dataWrapper = ((DeviceController*)dCtrl)->getLiveCDataWrapperPtr();
                if(dataWrapper && dataWrapper->hasKey(dsAttrName)) {
                    DataType::DataType attributeType;
                    string attributesName = dsAttrName;
                    err = ((DeviceController*)dCtrl)->getDeviceAttributeType(attributesName, attributeType);
                    if(err == 0){
                        switch (attributeType) {
                            case DataType::TYPE_INT64:
                                tmpString = boost::lexical_cast<string>(dataWrapper->getInt64Value(dsAttrName));
                                *dsAttrValue = convertStringToCharPtr(tmpString);
                                break;
                                
                            case DataType::TYPE_INT32:
                                tmpString = boost::lexical_cast<string>(dataWrapper->getInt64Value(dsAttrName));
                                *dsAttrValue = convertStringToCharPtr(tmpString);
                                break;
                                
                            case DataType::TYPE_DOUBLE:
                                tmpString = boost::lexical_cast<string>(dataWrapper->getDoubleValue(dsAttrName));
                                *dsAttrValue = convertStringToCharPtr(tmpString);
                                break;
                                
                            case DataType::TYPE_STRING:
                                tmpString = boost::lexical_cast<string>(dataWrapper->getStringValue(dsAttrName));
                                *dsAttrValue = convertStringToCharPtr(tmpString);
                                break;
                                
                            default:
                                err = 1;
                        }
                    }
                } else {
                  err = -1001;
                }
            } else {
                err = -1000;
            }
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int deinitToolkit() {
        int err = 0;
        try{
            ChaosUIToolkit::getInstance()->deinit();
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
}