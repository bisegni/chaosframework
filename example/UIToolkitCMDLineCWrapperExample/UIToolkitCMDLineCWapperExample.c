/*
 *	UIToolkitCMDLineExample.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <chaos/ui_toolkit/ChaosUIToolkitCWrapper.h>

int main (int argc, char* argv[] )
{
    if (argc != 4) return -1;
    int err = 0;
    char tmpInitString[256];
    uint32_t devID;
    const char *mdsServer = argv[1];
    const char *devName = argv[2];
    const char *attributeName = argv[3];
    char * attributeValue = NULL;
    
        //create init string
    sprintf(tmpInitString, "metadata-server=%s\nlog-on-console=true", mdsServer);
    
        //init the toolkit
    err = initToolkit(tmpInitString);
    if (err != 0) {
        printf("Error initToolkit %d", err);
        return -1;
    }
    
    err = getNewControllerForDeviceID(devName, &devID);
    if (err != 0) {
        printf("Error getNewControllerForDeviceID %d", err);
        return -1;
    }
    
    err = initDevice(devID);
    if (err != 0) {
        printf("Error initDevice %d", err);
        return -1;
    }
    
    err = startDevice(devID);
    if (err != 0) {
        printf("Error startDevice %d", err);
        return -1;
    }
    
    err = getStrValueForAttribute(devID, attributeName, &attributeValue);
    if (err != 0) {
        printf("Error getStrValueForAttribute %d", err);
    }else {
        if(attributeValue) {
            printf("Got the value for %s with value %s", attributeName, attributeValue);
            free(attributeValue);
        }
    }
    
    err = stopDevice(devID);
    if (err != 0) {
        printf("Error stopDevice %d", err);
        return -1;
    }
    
    err = deinitDevice(devID);
    if (err != 0) {
        printf("Error deinitDevice %d", err);
        return -1;
    }
    
    err = deinitController(devID);
    if (err != 0) {
        printf("Error deinitController %d", err);
        return -1;
    }
    
    err = deinitToolkit();
    if (err != 0) {
        printf("Error deinitToolkit %d", err);
        return -1;
    }
    return 0;
}
