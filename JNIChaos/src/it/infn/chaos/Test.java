/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package it.infn.chaos;
import java.util.Stack;
import java.util.Vector;

import it.infn.chaos.type.IntReference;

/**
 *
 * @author paolo
 */
public class Test {
    public static void main (String [] args) {
    
        System.out.println((System.getProperty("java.library.path")));
        JNIChaos jni=new JNIChaos();
        IntReference ir = new IntReference();
        IntReference cmd_id = new IntReference();
        Vector<String> attributeNames = new Stack<String>();
        String deviceIDString="sc-sin-a";
        StringBuffer tmpSB = new StringBuffer();
        
        int err = jni.initToolkit("metadata-server=pcbisegni:5000\nlog-on-console=true");
        if(err != 0) return;
        
        err = jni.getNewControllerForDeviceID(deviceIDString, ir);
        if(err != 0) {
        	jni.deinitToolkit();
        	return;
        }

        err = jni.getDeviceDatasetAttributeNameForDirection(ir.getValue(), 1, attributeNames);
        if(err != 0) {
        	jni.deinitToolkit();
        	return;
        }
        System.out.println("Output channels:");
        for (String attributeName : attributeNames) {
			System.out.println("\t\t" + attributeName);
		}
        
        attributeNames.clear();
        err = jni.getDeviceDatasetAttributeNameForDirection(ir.getValue(), 0, attributeNames);
        if(err != 0) {
        	jni.deinitToolkit();
        	return;
        }
        System.out.println("Input channels:");
        for (String attributeName : attributeNames) {
			System.out.println("\t\t" + attributeName);
		}
        
        err = jni.initDevice(ir.getValue());
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        err = jni.startDevice(ir.getValue());
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        System.out.println("Fetch sytem dataset:");
        err = jni.fetchLiveDatasetByDomain(ir.getValue(), 3);
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        System.out.println("print json dataset for system domain:");
        err = jni.getJSONDescriptionForDataset(ir.getValue(), 3, tmpSB);
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        System.out.println(tmpSB.toString());

        err = jni.setDeviceRunScheduleDelay(ir.getValue(), 200);
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        System.out.println("get sinWave Parameter:");
        tmpSB.setLength(0);
        err = jni.fetchLiveData(ir.getValue());
        err = jni.getStrValueForAttribute(ir.getValue(), "sinWave", tmpSB);
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        System.out.println(tmpSB.toString());
        
        System.out.println("set point value");
        err = jni.setStrValueForAttribute(ir.getValue(), "points", "1500");
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        System.out.println("send slow command");
        String t = "corr_test";
        err = jni.submitSlowControlCommand(ir.getValue(), t, 0, 50, cmd_id, 0, 0, null);
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        System.out.println("Fetch input dataset:");
        err = jni.fetchLiveDatasetByDomain(ir.getValue(), 1);
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        System.out.println("print json dataset for input domain:");
        tmpSB.setLength(0);
        err = jni.getJSONDescriptionForDataset(ir.getValue(), 1, tmpSB);
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        System.out.println(tmpSB.toString());
        
        
        
        err = jni.stopDevice(ir.getValue());
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        err = jni.deinitDevice(ir.getValue());
        if(err < 0) {
        	jni.deinitToolkit();
        	return;
        }
        
        jni.deinitController(ir.getValue());
        
        jni.deinitToolkit();
    }
}