/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package it.infn.chaos;
import it.infn.chaos.type.IntReference;

/**
 *
 * @author paolo
 */
public class Test {
    public static void main (String [] args) {
    
        System.out.println("Main, classe Test.java");
        JNIChaos jni=new JNIChaos();
        IntReference ir=new IntReference();
        String deviceIDString="deviceID";
        int devID=123456;
        
//Per richiamare le funzioni native:    jni.nomeMetodo(---);

        
        
//        System.out.println(jni.initToolkit(""));
//        System.out.println(jni.getNewControllerForDeviceID(deviceIDString, ir)+" : valore assegnato alla variabile intValue di IntReference");
//        System.out.println(jni.setControllerTimeout(devID, 0));
//        System.out.println(jni.getDeviceDatasetAttributeNameForDirection(devID, devID, deviceIDString, ir));
//        System.out.println(jni.initDevice(devID));
//        System.out.println(jni.startDevice(devID));
//        System.out.println(jni.setDeviceRunScheduleDelay(devID, devID));
//        System.out.println(jni.stopDevice(devID));
//        System.out.println(jni.deinitDevice(devID));
//        System.out.println(jni.fetchLiveData(devID));
//        System.out.println(jni.getTimeStamp(devID, ir));
//        System.out.println(jni.getStrValueForAttribute(devID, deviceIDString, deviceIDString));
//        System.out.println(jni.getStrValueForAttr(devID, deviceIDString, deviceIDString));
//        System.out.println(jni.setStrValueForAttribute(devID, deviceIDString, deviceIDString));
//        System.out.println(jni.submitSlowControlCommand(devID, deviceIDString, devID, devID, ir, devID, devID, deviceIDString));
//        System.out.println(jni.deinitController(devID));
//        System.out.println(jni.deinitToolkit());

    }
}