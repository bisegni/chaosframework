/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.infn.chaos;

import java.util.Vector;

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
    native int initToolkit(String initParam);
    
        //! Device Controller creation
    native int getNewControllerForDeviceID(String deviceID, IntReference devIDPtr);

    	//!
    native int setControllerTimeout(int devID, int timeout);
    
        //! Get device attribute names
    native int getDeviceDatasetAttributeNameForDirection(int devID, int attributeDirection, Vector<String> attributeNames);
    
        //! Device initialization
    native int initDevice(int devID);
    
        //! Startup the device
    native int startDevice(int devID);
    
        //! Setup of the run method delay
    native int setDeviceRunScheduleDelay(int devID, int delayTimeInMilliseconds);
    
        //! Stop the device
    native int stopDevice(int devID);
    
        //! Deinit the device
    native int deinitDevice(int devID);
    
        //! Fetch the device live data
    native int fetchLiveData(int devID);
    
    	//!fetch the live data for the required dataset
    native int fetchLiveDatasetByDomain(int devID, int domainType);
    
    	//!return the json representation of the dataset of the needed domain
    native int getJSONDescriptionForDataset(int devID, int domainType, StringBuffer jsonDescription);
    
    	//! Return a string represetnation of an attribute value
    native int getStrValueForAttribute(int devID, String dsAttrName, StringBuffer dsAttrValueHandle);
    
       //! Set the value for an attribute
    native int setStrValueForAttribute(int devID, String dsAttrName, String dsAttrValueCStr);
    
    	//!return the timestamp of the last fetched output dataset
    native int getTimeStamp(int devID,IntReference ts);
    
        //! Submit a new slow command
    native int submitSlowControlCommand(int dev_id,
                                        String command_alias,
                                        int submissione_rule,
                                        int priority,
                                        IntReference command_id,
                                        int scheduler_steps_delay,
                                        int submission_checker_steps_delay,
                                        String slow_command_data);
        
        //! Device Control deinitialization
    native int deinitController(int devID);
    
        //! UiToolkit deinitialization
    native int deinitToolkit();
}