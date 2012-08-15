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
    
        //! Get device attribute names
    /*!
     Retrive the device attributes name giving a device identification string and a a direction.
     \param deviceID is the unique string identification for a device.
     \param attributeDirection can be one of these value:
     - 0 -> input
     - 1 -> output
     - 2 -> bidirectional
     \param attributeNameArrayPtr isthe handle to an array of "c" string
     \param attributeNumber a pointer to an interger that will be filled
     with the number for attribute names found
     \return the error of operation, if all goes well the result is 0
     */
    int getDeviceDatasetAttributeNameForDirection(uint32_t devID, int16_t attributeDirection, char**attributeNameArrayHandle, uint32_t *attributeNumberPtr);

        //! Device initialization
    /*!
     Perform the initialization of the device, using the setup specified in the metadataserver
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    int initDevice(uint32_t devID);
    
        //! Startup the device
    /*!
     Perform the startup of the device
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    int startDevice(uint32_t devID);
    
        //! Setup of the run method delay
    /*!
     Setup of the delay of the run method schedule
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param delayTimeInMilliseconds the number of millisecond between lat run method execution
     \return the error of operation, if all goes well the result is 0
     */
    int setDeviceRunScheduleDelay(uint32_t devID, int32_t delayTimeInMilliseconds);
    
        //! Stop the device
    /*!
     Perform the interruption of the run method scehduling without deinitlize the control unit
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    int stopDevice(uint32_t devID);
    
        //! Deinit the device
    /*!
     Perform the deinitialization of the control unit
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    int deinitDevice(uint32_t devID);
    
        //! Fetch the device live data
    /*!
     Perform the update of device control internal live data  cache
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    int fetchLiveData(uint32_t devID);
    
        //! Return a string represetnation of an attribute value
    /*!
     Return the stirng representation of an attribute value. The current attribute value is keept from the
     device control interna cache, that is filled with \link fetchLiveData \endlink 
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param dsAttrName the name of the attribute
     \param dsAttrValueHandle the handle to c string that will contain the attribute value
     \return the error of operation, if all goes well the result is 0
     */
    int getStrValueForAttribute(uint32_t devID, const char * const dsAttrName, char ** dsAttrValueHandle);
    
        //! Set the value for an attribute
    /*!
     Set an attribute value using  a string reprpesentation for the new value to use.
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param dsAttrName the name of the attribute
     \param dsAttrValueCStr the "c" string representing the attribute value
     \return the error of operation, if all goes well the result is 0
     */
    int setStrValueForAttribute(uint32_t devID, const char * const dsAttrName, const char * const dsAttrValueCStr);
    
        //! Device Control deinitialization
    /*!
     Perform deinitialization of a device control associated to an id
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    int deinitController(uint32_t devID);
    
        //! UiToolkit deinitialization
    /*!
     Perform the deinitialization of UIToolkit
     \return the error of operation, if all goes well the result is 0
     */
    int deinitToolkit();
#ifdef __cplusplus
}
#endif

    /** @} */ // end of UIToolkitCWrapper
#endif
