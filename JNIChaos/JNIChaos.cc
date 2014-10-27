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
  }

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    setControllerTimeout
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_setControllerTimeout
  (JNIEnv *env, jobject obj, jint devID, jint timeout){
    return setControllerTimeout((uint32_t)devID, (uint32_t)timeout);
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

    int result=getDeviceDatasetAttributeNameForDirection((uint32_t)devID,
							 (int16_t)attributeDirection,
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
    return initDevice((uint32_t)devID);
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    startDevice
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_startDevice
  (JNIEnv * env, jobject obj, jint devID){
    return startDevice((uint32_t)devID);
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    setDeviceRunScheduleDelay
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_setDeviceRunScheduleDelay
  (JNIEnv *env, jobject obj, jint devID, jint delayTimeInMilliseconds){
    return setDeviceRunScheduleDelay((uint32_t)devID, (int32_t)delayTimeInMilliseconds);
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    stopDevice
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_stopDevice
  (JNIEnv *env, jobject obj, jint devID){
    return stopDevice((uint32_t)devID);
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    deinitDevice
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_deinitDevice
  (JNIEnv *env, jobject obj, jint devID){
    return deinitDevice((uint32_t)devID);
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    fetchLiveData
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_fetchLiveData
  (JNIEnv *env, jobject obj, jint devID){
    return fetchLiveData((uint32_t)devID);
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    fetchLiveDatasetByDomain
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_fetchLiveDatasetByDomain
  (JNIEnv *env, jobject obj, jint devID, jint domainType){
    return fetchLiveDatasetByDomain((uint32_t)devID, (uint16_t)domainType);
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    getJSONDescriptionForDataset
 * Signature: (IILjava/lang/StringBuffer;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_getJSONDescriptionForDataset
  (JNIEnv *env, jobject obj, jint devID, jint domainType, jobject jsonDescription){
    char * json_dataset_handler;
    int result = getJSONDescriptionForDataset(devID, domainType, &json_dataset_handler);
    if(!result && json_dataset_handler) {
      //get class reppresentation for string buffer
      jclass str_buff_obj = env->GetObjectClass (jsonDescription);

      //get reference to append method
      jmethodID str_buff_obj_append = env->GetMethodID (str_buff_obj, "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
      if (str_buff_obj_append != 0) {
        //create jstring
        jstring _json_desc = env->NewStringUTF ((const char *) json_dataset_handler);

        //append to string buffer
        env->CallObjectMethod(jsonDescription, str_buff_obj_append, _json_desc);
      }

      //delete the string
      free(json_dataset_handler);
    }
    return result;
}


/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    getStrValueForAttribute
 * Signature: (ILjava/lang/String;Ljava/lang/StringBuffer;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_getStrValueForAttribute
  (JNIEnv *env, jobject obj, jint devID, jstring dsAttrNameString, jobject dsAttrValueString){

    char * attribute_value = NULL;
    const char * const dsAttrName = env->GetStringUTFChars(dsAttrNameString,0);
    //get attribute value to c string
    int result = getStrValueForAttribute((uint32_t)devID, dsAttrName, &attribute_value);

    if(attribute_value) {
      //write string parameter in string buffer

      //get class reppresentation for string buffer
      jclass str_buff_obj = env->GetObjectClass (dsAttrValueString);
      if(str_buff_obj) {
        //get reference to append method
        jmethodID str_buff_obj_append = env->GetMethodID (str_buff_obj, "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
        if(str_buff_obj_append) {
          //create jstring
          jstring _attribute_value = env->NewStringUTF ((const char *) attribute_value);

          //append to string buffer
          env->CallObjectMethod(dsAttrValueString, str_buff_obj_append, _attribute_value);
        }
      }
      free(attribute_value);
    }

    //release attribute name c string
    if(dsAttrName) env->ReleaseStringUTFChars(dsAttrNameString, dsAttrName);
    return result;
//    return 10; // Debug
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

    int result = setStrValueForAttribute((uint32_t)devID, dsAttrName, dsAttrValueCStr);

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
    tsPtrUint64Value=(uint64_t)tsPtrIntValue;
    tsPtrUint64=&tsPtrUint64Value;
    return getTimeStamp((uint32_t)devID, tsPtrUint64);
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    submitSlowControlCommand
 * Signature: (ILjava/lang/String;IILit/infn/chaos/type/IntReference;IILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_submitSlowControlCommand
  (JNIEnv *env, jobject obj, jint devID, jstring commandAliasString, jint submissioneRule,
   jint priority, jobject commandIDPtr, jint schedulerStepsDelay, jint submissionCheckerStepsDelay, jstring slowCommandDataString){
printf("begin:\n");
fflush(stdout);
    const char * commandAlias = env->GetStringUTFChars(commandAliasString, 0);
printf("commandAlias:");printf(commandAlias);printf("\n");
fflush(stdout);
    const char * slowCommandData = NULL;
    printf("get slowCommandData size \n");
    fflush(stdout);
    if( slowCommandDataString && env->GetStringLength(slowCommandDataString) ) {
      printf("we have command data of %d", env->GetStringLength(slowCommandDataString));printf("\n");
      fflush(stdout);
      slowCommandData = env->GetStringUTFChars(slowCommandDataString, 0);
      printf("slowCommandData:");printf(slowCommandData);printf("\n");
    }else {
      printf("no command data");
    }

  fflush(stdout);
  printf("\ncommand id preparation:\n");
fflush(stdout);
    uint64_t command_id = 0;
    jclass clazz = env->GetObjectClass(commandIDPtr);
    jmethodID mid = env->GetMethodID(clazz, "setValue", "(I)V");

printf("call api:\n");
fflush(stdout);

    int result = submitSlowControlCommand((uint32_t)devID,
					    commandAlias,
					    (uint16_t)submissioneRule,
					    (uint32_t)priority,
					    &command_id,
					    (uint32_t)schedulerStepsDelay,
					    (uint32_t)submissionCheckerStepsDelay,
					    slowCommandData);

    env->CallIntMethod(commandIDPtr, mid, command_id);

    env->ReleaseStringUTFChars(commandAliasString, commandAlias);
    env->ReleaseStringUTFChars(slowCommandDataString, slowCommandData);
    return result;
}

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    deinitController
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_deinitController
  (JNIEnv *env, jobject obj, jint devID) {
    return deinitController((uint32_t)devID);
  }

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    deinitToolkit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_deinitToolkit
  (JNIEnv *env, jobject obj) {
    return deinitToolkit();
  }
