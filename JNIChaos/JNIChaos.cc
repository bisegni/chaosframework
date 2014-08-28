#include <chaos/ui_toolkit/ChaosUIToolkitCWrapper.h>
#include "JNIChaos.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    initToolkit
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_initToolkit
  (JNIEnv *env, jobject obj, jstring initParamString) {

    const char * initParam = env->GetStringUTFChars(initParamString, 0);
    int result = initToolkit(initParam);
    env->ReleaseStringUTFChars(initParamString, initParam);
    return result;
//    return 1; // Debug
  }

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    getNewControllerForDeviceID
 * Signature: (Ljava/lang/String;Lit/infn/chaos/type/IntReference;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_getNewControllerForDeviceID
  (JNIEnv *env, jobject obj, jstring deviceIdString, jobject devIdPtr) {
    uint32_t device_id = 0;
    const char * const deviceId = env->GetStringUTFChars(deviceIdString,0);

    jclass clazz = env->GetObjectClass(devIdPtr);
    jmethodID mid = env->GetMethodID(clazz, "setValue", "(I)V");

    int result = getNewControllerForDeviceID(deviceId, &device_id);

    env->CallIntMethod(devIdPtr, mid, device_id);
    env->ReleaseStringUTFChars(deviceIdString, deviceId);
    return result;
//    return devIdPtrIntValue;  // Debug
  }

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    setControllerTimeout
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_setControllerTimeout
  (JNIEnv *env, jobject obj, jint devID, jint timeout){

    return setControllerTimeout(uint32_t(devID), uint32_t(timeout));
//    return 2; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    getDeviceDatasetAttributeNameForDirection
 * Signature: (IILjava/util/Vector;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_getDeviceDatasetAttributeNameForDirection
  (JNIEnv *env, jobject obj, jint devID, jint attributeDirection, jobject attributeNamesVector) {

    jclass vectorClass = env->FindClass("java/util/Vector");
    jmethodID vectorAddElement = env->GetMethodID(vectorClass, "addElement", "(Ljava/lang/Object;)V");

    char * * attribute_name_array = NULL;
    uint32_t attribute_name_array_size;

    int result=getDeviceDatasetAttributeNameForDirection(uint32_t(devID),
							 int16_t(attributeDirection),
							 &attribute_name_array,
							 &attribute_name_array_size);

    if(attribute_name_array) {
      for(int idx = 0;
        idx < attribute_name_array_size;
        idx++) {
          //create java string
          jstring attribute_name = env->NewStringUTF((const char *) attribute_name_array[idx]);

          //add java string to the vector
          env->CallIntMethod(attributeNamesVector, vectorAddElement, attribute_name);

          //delete memory for attribute name
          free(attribute_name_array[idx]);
      }

      // delete array
      free(attribute_name_array);
    }

    return result;
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    initDevice
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_initDevice
  (JNIEnv * env, jobject obj, jint devID){

    return initDevice(uint32_t(devID));
//    return 4; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    startDevice
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_startDevice
  (JNIEnv * env, jobject obj, jint devID){

    return startDevice(uint32_t(devID));
//    return 5; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    setDeviceRunScheduleDelay
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_setDeviceRunScheduleDelay
  (JNIEnv *env, jobject obj, jint devID, jint delayTimeInMilliseconds){

    return setDeviceRunScheduleDelay(uint32_t(devID), int32_t(delayTimeInMilliseconds));
//    return 6; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    stopDevice
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_stopDevice
  (JNIEnv *env, jobject obj, jint devID){

    return stopDevice(uint32_t(devID));
//    return 7; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    deinitDevice
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_deinitDevice
  (JNIEnv *env, jobject obj, jint devID){

    return deinitDevice(uint32_t(devID));
//    return 8; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    fetchLiveData
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_fetchLiveData
  (JNIEnv *env, jobject obj, jint devID){

    return fetchLiveData(uint32_t(devID));
//    return 9; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    getStrValueForAttribute
 * Signature: (ILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_getStrValueForAttribute
  (JNIEnv *env, jobject obj, jint devID, jstring dsAttrNameString, jstring dsAttrValueHandleString){

    const char * const dsAttrName = env->GetStringUTFChars(dsAttrNameString,0);
    char * dsAttrValueHandle = (char*)env->GetStringUTFChars(dsAttrValueHandleString,0);
    char * dsAttrValueHandlePtr=(char*)malloc(strlen(dsAttrValueHandle)+1);
    strcpy(dsAttrValueHandlePtr,dsAttrValueHandle);
    char ** dsAttrValueHandlePtr2=&dsAttrValueHandlePtr;

    int result = getStrValueForAttribute(uint32_t(devID), dsAttrName, dsAttrValueHandlePtr2);
    env->ReleaseStringUTFChars(dsAttrNameString, dsAttrName);
    env->ReleaseStringUTFChars(dsAttrValueHandleString, dsAttrValueHandle);
    free(dsAttrValueHandlePtr);
    return result;
//    return 10; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    getStrValueForAttr
 * Signature: (ILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_getStrValueForAttr
  (JNIEnv *env, jobject obj, jint devID, jstring dsAttrNameString, jstring dsAttrValueHandleString){

    const char * const dsAttrName = env->GetStringUTFChars(dsAttrNameString,0);
    char * dsAttrValueHandle = (char*)env->GetStringUTFChars(dsAttrValueHandleString,0);

    int result = getStrValueForAttr(uint32_t(devID), dsAttrName, dsAttrValueHandle);
    env->ReleaseStringUTFChars(dsAttrNameString, dsAttrName);
    env->ReleaseStringUTFChars(dsAttrValueHandleString, dsAttrValueHandle);
    return result;
//    return 11; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    setStrValueForAttribute
 * Signature: (ILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_setStrValueForAttribute
  (JNIEnv *env, jobject obj, jint devID, jstring dsAttrNameString, jstring dsAttrValueCStrString){

    const char * const dsAttrName = env->GetStringUTFChars(dsAttrNameString,0);
    const char * const dsAttrValueCStr = env->GetStringUTFChars(dsAttrValueCStrString,0);

    int result = setStrValueForAttribute(uint32_t(devID), dsAttrName, dsAttrValueCStr);
    env->ReleaseStringUTFChars(dsAttrNameString, dsAttrName);
    env->ReleaseStringUTFChars(dsAttrValueCStrString, dsAttrValueCStr);
    return result;
//    return 12; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    getTimeStamp
 * Signature: (ILit/infn/chaos/type/IntReference;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_getTimeStamp
  (JNIEnv *env, jobject obj, jint devID, jobject tsPtr){

    jclass clazz = env->GetObjectClass(tsPtr);
    jmethodID mid = env->GetMethodID(clazz, "getValue", "()I");
    jint tsPtrIntValue = env->CallIntMethod(tsPtr, mid);
    uint64_t tsPtrUint64Value, *tsPtrUint64;
    tsPtrUint64Value=uint64_t(tsPtrIntValue);
    tsPtrUint64=&tsPtrUint64Value;

    return getTimeStamp(uint32_t(devID), tsPtrUint64);
//    return 13; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    submitSlowControlCommand
 * Signature: (ILjava/lang/String;IILit/infn/chaos/type/IntReference;IILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_submitSlowControlCommand
  (JNIEnv *env, jobject obj, jint devID, jstring commandAliasString, jint submissioneRule,
   jint priority, jobject commandIDPtr, jint schedulerStepsDelay, jint submissionCheckerStepsDelay, jstring slowCommandDataString){

    const char * const commandAlias = env->GetStringUTFChars(commandAliasString,0);
    const char * const slowCommandData = env->GetStringUTFChars(slowCommandDataString,0);

    jclass clazz = env->GetObjectClass(commandIDPtr);
    jmethodID mid = env->GetMethodID(clazz, "getValue", "()I");
    jint commandIDPtrIntValue = env->CallIntMethod(commandIDPtr, mid);
    uint64_t commandIDPtrUint64Value, *commandIDPtrUint64;
    commandIDPtrUint64Value=uint64_t(commandIDPtrIntValue);
    commandIDPtrUint64=&commandIDPtrUint64Value;

    int result = submitSlowControlCommand(uint32_t(devID),
					    commandAlias,
					    uint16_t(submissioneRule),
					    uint32_t(priority),
					    commandIDPtrUint64,
					    uint32_t(schedulerStepsDelay),
					    uint32_t(submissionCheckerStepsDelay),
					    slowCommandData);

    env->ReleaseStringUTFChars(commandAliasString, commandAlias);
    env->ReleaseStringUTFChars(slowCommandDataString, slowCommandData);
    return result;
//    return 14; // Debug
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    deinitController
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_deinitController
  (JNIEnv *env, jobject obj, jint devID) {

    return deinitController(uint32_t(devID));
//    return 15; // Debug
  }

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    deinitToolkit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_deinitToolkit
  (JNIEnv *env, jobject obj) {

    return deinitToolkit();
//    return 16; // Debug
  }
