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

#ifndef CHAOSFramework_UIToolkitCWrapper_h
#define CHAOSFramework_UIToolkitCWrapper_h

typedef void DevCtrl;

#ifdef __cplusplus
extern "C" {
#endif
    int initToolkit(const char* startupParameter);
    DevCtrl * getNewControllerForDeviceID(const char* deviceID);
    int initDevice(DevCtrl *dCtrl);
    int startDevice(DevCtrl *dCtrl);
    int stopDevice(DevCtrl *dCtrl);
    int deinitDevice(DevCtrl *dCtrl);
    int fetchLiveData(DevCtrl *dCtrl);
    int getStrValueForAttribute(DevCtrl *dCtrl, const char * dsAttrName, char ** dsAttrValue);
    int deinitToolkit();
#ifdef __cplusplus
}
#endif

#endif
