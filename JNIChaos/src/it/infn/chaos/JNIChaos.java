/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.infn.chaos;

import it.infn.chaos.type.IntReference;

/**
 *
 * @author bisegni
 */
public class JNIChaos {

    static {
        System.loadLibrary("jnichaos");
    }

        //! User interface toolkit initialization
    /*!
     Perform the user interface toolkit initialization, the startup parameter are the same of the complied
     !CHAOS application. For example: --log-on-console is setupped with the string
     "log-on-console=true" and for example "--metadata-server=host:5000" is "metadata-server=host:5000"
     \param startupParameter the startup initialization string queal to the norma launch program
     \return the error of operation, if all goes well the result is 0
     */    
    native int initToolkit(String initParam);
    
        //! Device Controller creation
    /*!
     Create an instance of the controller for a device. The function return the DeviceControl ID that
     need to be used in other funtion.
     \param deviceID is the unique string identification for a device.
     \param devIDPtr is ap ointer toan unsigned int32 used for return the id for the create channel
     \return the error of operation, if all goes well the result is 0
     */
    native int getNewControllerForDeviceID(String deviceID, IntReference devIDPtr);

    native int setControllerTimeout(int devID, int timeout);
    
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
    native int getDeviceDatasetAttributeNameForDirection(int devID, int attributeDirection,
            String attributeNameArrayHandle, IntReference attributeNumberPtr);
    
        //! Device initialization
    /*!
     Perform the initialization of the device, using the setup specified in the metadataserver
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    native int initDevice(int devID);
    
        //! Startup the device
    /*!
     Perform the startup of the device
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    native int startDevice(int devID);
    
        //! Setup of the run method delay
    /*!
     Setup of the delay of the run method schedule
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param delayTimeInMilliseconds the number of millisecond between lat run method execution
     \return the error of operation, if all goes well the result is 0
     */
    native int setDeviceRunScheduleDelay(int devID, int delayTimeInMilliseconds);
    
        //! Stop the device
    /*!
     Perform the interruption of the run method scehduling without deinitlize the control unit
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    native int stopDevice(int devID);
    
        //! Deinit the device
    /*!
     Perform the deinitialization of the control unit
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    native int deinitDevice(int devID);
    
        //! Fetch the device live data
    /*!
     Perform the update of device control internal live data  cache
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    native int fetchLiveData(int devID);
    
        //! Return a string represetnation of an attribute value
    /*!
     Return the stirng representation of an attribute value. The current attribute value is keept from the
     device control interna cache, that is filled with \link fetchLiveData \endlink
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param dsAttrName the name of the attribute
     \param dsAttrValueHandle the handle to c string that will contain the attribute value
     \return the error of operation, if all goes well the result is 0
     */
    native int getStrValueForAttribute(int devID, String dsAttrName, String dsAttrValueHandle);
    
    /*!
     Return the stirng representation of an attribute value. The current attribute value is keept from the
     device control interna cache, that is filled with \link fetchLiveData \endlink
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param dsAttrName the name of the attribute
     \param dsAttrValue a pointer to preallocated c string that will contain the attribute value
     \return the error of operation, if all goes well the result is 0
     */
    native int getStrValueForAttr(int devID, String dsAttrName, String dsAttrValue);
    
       //! Set the value for an attribute
    /*!
     Set an attribute value using  a string reprpesentation for the new value to use.
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \param dsAttrName the name of the attribute
     \param dsAttrValueCStr the "c" string representing the attribute value
     \return the error of operation, if all goes well the result is 0
     */
    native int setStrValueForAttribute(int devID, String dsAttrName, String dsAttrValueCStr);
    
    native int getTimeStamp(int devID,IntReference ts);
    
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
    native int submitSlowControlCommand(int dev_id,
                                        String command_alias,
                                        int submissione_rule,
                                        int priority,
                                        IntReference command_id,
                                        int scheduler_steps_delay,
                                        int submission_checker_steps_delay,
                                        String slow_command_data);

//#endif

        
        //! Device Control deinitialization
    /*!
     Perform deinitialization of a device control associated to an id
     \param devID is the identification number got from \link getNewControllerForDeviceID \endlink function
     \return the error of operation, if all goes well the result is 0
     */
    native int deinitController(int devID);
    
        //! UiToolkit deinitialization
    /*!
     Perform the deinitialization of UIToolkit
     \return the error of operation, if all goes well the result is 0
     */
    native int deinitToolkit();

}