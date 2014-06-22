#include "JNIChaos.h"
/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    initToolkit
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_initToolkit
  (JNIEnv *env, jobject obj, jstring str) {
    return 0;
  }

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    getNewControllerForDeviceID
 * Signature: (Ljava/lang/String;Lit/infn/chaos/type/IntReference;)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_getNewControllerForDeviceID
  (JNIEnv *env, jobject obj, jstring str1, jobject str2) {
    return 0;
  }

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    deinitController
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_deinitController
  (JNIEnv *env, jobject obj, jint deviceID) {
    return 0;
  }

/*
 * Class:     it_infn_chaos_JNIChaos
 * Method:    deinitToolkit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_it_infn_chaos_JNIChaos_deinitToolkit
  (JNIEnv *env, jobject obj) {
    return 0;
  }
