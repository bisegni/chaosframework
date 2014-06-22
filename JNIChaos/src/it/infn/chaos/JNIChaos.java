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

