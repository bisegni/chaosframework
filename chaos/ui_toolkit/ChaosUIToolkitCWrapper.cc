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

#include <map>
#include <string.h>
#include <memory.h>

#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/ChaosUIToolkitCWrapper.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <chaos/common/data/CDataWrapper.h>

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace boost;


uint32_t sequenceNumber = 0;
std::map<uint32_t, DeviceController *> chanelMap;

    //---------------------------------------------------------------
DeviceController *getDeviceControllerFromID(uint32_t did) {
    if(chanelMap.count(did)==0) return NULL;
    return chanelMap[did];
}

    //---------------------------------------------------------------
char * convertStringToCharPtr(string& orgString) {
    char *result = NULL;
    if(!orgString.length()) return NULL;
    
    const char *tmpCStr = orgString.c_str();
    result = (char*)malloc(strlen(tmpCStr)+1);
    strcpy(result, tmpCStr);
    return result;
}

extern "C" {
        //---------------------------------------------------------------
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
    
        //---------------------------------------------------------------
    int getNewControllerForDeviceID(const char * const deviceID, uint32_t *devIDPtr) {
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
	  printf("eccezione %s\n",e.what());
            err = e.errorCode;
        }
        return err;
    }
        //---------------------------------------------------------------
    int setControllerTimeout(uint32_t devID, uint32_t timeout) {
        int err = 0;
        DeviceController *controller = NULL;
        try{
            controller = getDeviceControllerFromID(devID);
            if(controller) {
                controller->setRequestTimeWaith(timeout);
            }
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
        //---------------------------------------------------------------
    int getDeviceDatasetAttributeNameForDirection(uint32_t devID, int16_t attributeDirection, char***attributeNameArrayHandle, uint32_t *attributeNumberPtr) {
        int err = 0;
        char **arrayPtr;
        uint32_t attributeFound = 0;
        vector<string> attributesName;
        DataType::DataSetAttributeIOAttribute _attributeDirection = static_cast<DataType::DataSetAttributeIOAttribute>(attributeDirection);
        try{
            if(devID) {
                DeviceController * ctrl = getDeviceControllerFromID(devID);
                if(ctrl){
                        //get the name
                    ctrl->getDeviceDatasetAttributesName(attributesName,  _attributeDirection);

                        //copy yhe array dimension on function param
                    *attributeNumberPtr = attributeFound = static_cast<uint32_t>(attributesName.size());
                    
                        //allcoate the memory for the array
                    *attributeNameArrayHandle = arrayPtr = (char**)malloc(attributeFound * sizeof(char*));
                    
                        //scann all name and allocate the string
                    for (int idx = 0 ; idx < attributeFound; idx++) {
                        string curName = attributesName[idx];
                        
                            //create and copy the attribute name into appropiate index
                        arrayPtr[idx] = (char*)malloc(strlen(curName.c_str()) * sizeof(char) + 1);
                        
                            //copy the string
                        strcpy(arrayPtr[idx], curName.c_str());
                    }
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
    
        //---------------------------------------------------------------
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
    
        //---------------------------------------------------------------
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
    
        //---------------------------------------------------------------
    int setDeviceRunScheduleDelay(uint32_t devID, int32_t delayTimeInMilliseconds) {
        int err = 0;
        try{
            if(devID) {
                DeviceController * ctrl = getDeviceControllerFromID(devID);
                if(ctrl){
                    err = ctrl->setScheduleDelay(delayTimeInMilliseconds);
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
    
        //---------------------------------------------------------------
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
    
        //---------------------------------------------------------------
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
    
        //---------------------------------------------------------------
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

  int getTimeStamp(uint32_t devID,uint64_t*ts){
       DeviceController *dCtrl = getDeviceControllerFromID(devID);
       if(dCtrl){
	 return dCtrl->getTimeStamp(*ts);
       }
       return -1;
    }
    
        //---------------------------------------------------------------
    int getStrValueForAttribute(uint32_t devID, const char * const dsAttrName, char ** dsAttrValueHandle) {
        int err = 0;
        string tmpString;
        try{
            DeviceController *dCtrl = getDeviceControllerFromID(devID);
            if(dCtrl && dsAttrName && dsAttrValueHandle) {
                CDataWrapper *  dataWrapper = ((DeviceController*)dCtrl)->getLiveCDataWrapperPtr();
                if(dataWrapper && dataWrapper->hasKey(dsAttrName)) {
                    DataType::DataType attributeType;
                    string attributesName = dsAttrName;
                    err = ((DeviceController*)dCtrl)->getDeviceAttributeType(attributesName, attributeType);
                    if(err == 0){
                        switch (attributeType) {
                            case DataType::TYPE_INT64:
                                tmpString = boost::lexical_cast<string>(dataWrapper->getInt64Value(dsAttrName));
                                *dsAttrValueHandle = convertStringToCharPtr(tmpString);
                                break;
                                
                            case DataType::TYPE_INT32:
                                tmpString = boost::lexical_cast<string>(dataWrapper->getInt64Value(dsAttrName));
                                *dsAttrValueHandle = convertStringToCharPtr(tmpString);
                                break;
                                
                            case DataType::TYPE_DOUBLE:
                                tmpString = boost::lexical_cast<string>(dataWrapper->getDoubleValue(dsAttrName));
                                *dsAttrValueHandle = convertStringToCharPtr(tmpString);
                                break;
                                
                            case DataType::TYPE_STRING:
                                tmpString = boost::lexical_cast<string>(dataWrapper->getStringValue(dsAttrName));
                                *dsAttrValueHandle = convertStringToCharPtr(tmpString);
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
  int getStrValueForAttr(uint32_t devID, const char * const dsAttrName, char * dsAttrValueHandle){
    char *pntS=0;
    int ret;
    ret=getStrValueForAttribute( devID, dsAttrName, &pntS);
    if(dsAttrValueHandle && pntS){
      strcpy(dsAttrValueHandle,pntS);
      free(pntS);
    } else {
      ret =-1;
    }
    return ret;
  }

    
        //---------------------------------------------------------------
    int setStrValueForAttribute(uint32_t devID, const char * const dsAttrName, const char * const dsAttrValueCStr) {
        int err = 0;
        string attributeName = dsAttrName;
        DeviceController *dCtrl = getDeviceControllerFromID(devID);
        if(dCtrl && dsAttrName && dsAttrValueCStr) {
			//changed to use the new api
            err = dCtrl->setAttributeToValue(dsAttrName, dsAttrValueCStr, false);
        } else {
            err = -1001;
        }
        return err;
    }
    
	 //---------------------------------------------------------------
	int submitSlowControlCommand(uint32_t dev_id,
								 const char * const command_alias,
								 uint16_t submissione_rule,
								 uint32_t priority,
								 uint64_t *command_id,
								 uint32_t scheduler_steps_delay,
								 uint32_t submission_checker_steps_delay,
								 const char * const slow_command_data) {
		int err = 0;
		uint64_t cmd_id_tmp;
        DeviceController *dCtrl = getDeviceControllerFromID(dev_id);
        if(dCtrl && command_alias) {
			//changed to use the new api
			std::auto_ptr<chaos::common::data::CDataWrapper> data_wrapper;
			
			std::string cmd_alias_str = command_alias;
			if(slow_command_data) {
				data_wrapper.reset(new chaos::common::data::CDataWrapper());
				if(data_wrapper.get())
					data_wrapper->setSerializedJsonData(slow_command_data);
				else
					return -1001;
			}
			
            err = dCtrl->submitSlowControlCommand(cmd_alias_str,
												  static_cast<chaos::common::batch_command::SubmissionRuleType::SubmissionRule>(submissione_rule),
												  priority,
												  cmd_id_tmp,
                                                  0,
												  scheduler_steps_delay,
												  submission_checker_steps_delay,
												  data_wrapper.get());
			if(!err && command_id) {
				*command_id = cmd_id_tmp;
			}
        } else {
            err = -1002;
        }
        return err;
	}
	
        //---------------------------------------------------------------
    int deinitController(uint32_t devID) {
        int err = 0;
        try{
            if(devID) {
                DeviceController * ctrl = getDeviceControllerFromID(devID);
                if(ctrl){
                    HLDataApi::getInstance()->disposeDeviceControllerPtr(ctrl);
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
    
        //---------------------------------------------------------------
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
