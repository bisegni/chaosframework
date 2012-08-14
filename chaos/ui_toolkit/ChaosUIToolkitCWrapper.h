/*
 *	ChaosUIToolkitCWrapper.h
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

/*! \file ChaosUIToolkitCWrapper*/

/*! \page page_uicwrapper The Chaos "C Wrapper" for User Interface Toolkit
 *  \section page_cuit_sec Why an a "C" language wrapper?
 *  Some applications used in scentific experiment (as for example LabView) can be
 *  can be customized with the use of shared library written in "C" language. At this
 *  scope, !CHOAS give a simple interface to a DeviceController managed by user interface
 *  toolkit \link UIToolkitCWrapper wrapped with few simple "C" function\endlink. 
 */

/** @defgroup UIToolkitCWrapper "C" language interface to user interface toolkit
 *  These are the funciton to be used with the "C" language to use cpp UIToolkit library.
 *  The library to be linked is the same as the cpp language, libchoas_uitoolkit
 *  @{
 */

#ifndef CHAOSFramework_UIToolkitCWrapper_h
#define CHAOSFramework_UIToolkitCWrapper_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    
        //! User interface toolkit initialization
    /*!
     Perform the user interface toolkit initialization, the startup parameter are the same of the complied 
     !CHAOS application. For example: --log-on-console is setupped with the string
     "log-on-console=true" and for example "--metadata-server=host:5000" is "metadata-server=host:5000"
     \param startupParameter the startup initialization string queal to the norma launch program
     \return the error of operation, if all goes well the result is 0
     */
    int initToolkit(const char* startupParameter);
    
        //! Device Controller creation
    /*!
     Create an instance of the controller for a device. The function return the DeviceControl ID that
     need to be used in other funtion.
     \param deviceID is the unique string identification for a device.
     \param devIDPtr is ap ointer toan unsigned int32 used for return the id for the create channel
     \return the error of operation, if all goes well the result is 0
     */
    int getNewControllerForDeviceID(const char * const deviceID, uint32_t *devIDPtr);
    
        //! Device initialization
    /*!
     Perform the initialization of the device, using the setup specified in the metadataserver
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    int initDevice(uint32_t devID);
    int startDevice(uint32_t devID);
    int setDeviceRunScheduleDelay(uint32_t devID, int32_t delayTimeInMilliseconds);
    int stopDevice(uint32_t devID);
    int deinitDevice(uint32_t devID);
    int fetchLiveData(uint32_t devID);
    int getStrValueForAttribute(uint32_t devID, const char * const dsAttrName, char ** dsAttrValue);
    int setStrValueForAttribute(uint32_t devID, const char * const dsAttrName, const char * const dsAttrValue);
    int deinitController(uint32_t devID);
    int deinitToolkit();
#ifdef __cplusplus
}
#endif

    /** @} */ // end of UIToolkitCWrapper
#endif
