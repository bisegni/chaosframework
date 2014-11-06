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
#ifdef LABVIEW
// labview unable to parse stdint
typedef unsigned long uint32_t;
typedef int int32_t;

typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned long long uint64_t;
#endif
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

    int setControllerTimeout(uint32_t devID, uint32_t timeout);

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
    int getDeviceDatasetAttributeNameForDirection(uint32_t devID, int16_t attributeDirection, char***attributeNameArrayHandle, uint32_t *attributeNumberPtr);

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

		//! fetch the live data for the required dataset
	/*!
	 \param domain_type is the the domain the need to be fetched from the chaos
	 data chache: 0=DatasetDomainOutput, 1=DatasetDomainInput, 2=DatasetDomainCustom, 3=DatasetDomainSystem
	 */
	int fetchLiveDatasetByDomain(uint32_t devID,
								 int16_t domain_type);
	
	//! return the json representation of the dataset of the needed domain
	/*!
	 the json representation is got form the latest live dataaset for the domain updated by the
	 last call to the fetchLiveDatasetByDomain api.
	 */
	int getJSONDescriptionForDataset(uint32_t devID,
									 int16_t domain_type,
									 char ** json_dataset_handler);
	
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

 /*!
     Return the stirng representation of an attribute value. The current attribute value is keept from the
     device control interna cache, that is filled with \link fetchLiveData \endlink
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param dsAttrName the name of the attribute
     \param dsAttrValue a pointer to preallocated c string that will contain the attribute value
     \return the error of operation, if all goes well the result is 0
     */
    int getStrValueForAttr(uint32_t devID, const char * const dsAttrName, char * dsAttrValue);

        //! Set the value for an attribute
    /*!
     Set an attribute value using  a string reprpesentation for the new value to use.
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param dsAttrName the name of the attribute
     \param dsAttrValueCStr the "c" string representing the attribute value
     \return the error of operation, if all goes well the result is 0
     */
    int setStrValueForAttribute(uint32_t devID, const char * const dsAttrName, const char * const dsAttrValueCStr);

  int getTimeStamp(uint32_t devID,uint64_t*ts);
	//! Submit a new slow command
	/*!
	 \ingroup API_Slow_Control
	 The submition of slow command is made collection all the information that permit to submit it
	 \param dev_id is the identification number got from \link getNewControllerForDeviceID \endlink function
	 \param command_alias represent the alias of the command the the control unit expost from RPC subsystem
	 \param submissione_rule determinate the rule with which the command is submitted. @SubmissionRuleType::SubmissionRule
	 \param command_id is the assigned command id to the submitted one
	 the execution of the current execution command in the control unit, according with his running state
	 \param scheduler_steps_delay rapresent the intervall beetween the step of the scehduler [...acquisition -> correlation -> scheduleInterval...]
	 \param submission_checker_steps_delay is the delay between two steps of the submission checker
	 \param slow_command_data is the abstraction of the command data that is passed to the set handler befor the scheduler loop of the new command
	 take palce. The memory of that parameter is not free
	 */
/*#ifdef __cplusplus
	int submitSlowControlCommand(uint32_t dev_id,
								 const char * const command_alias,
								 uint16_t submissione_rule,
								 uint32_t priority,
								 uint64_t *command_id,
								 uint32_t scheduler_steps_delay = 0,
								 uint32_t submission_checker_steps_delay = 0,
								 const char * const slow_command_data = NULL);
#else*/
	int submitSlowControlCommand(uint32_t dev_id,
								 const char * const command_alias,
								 uint16_t submissione_rule,
								 uint32_t priority,
								 uint64_t *command_id,
								 uint32_t scheduler_steps_delay ,
								 uint32_t submission_checker_steps_delay ,
								 const char * const slow_command_data );

//#endif
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
