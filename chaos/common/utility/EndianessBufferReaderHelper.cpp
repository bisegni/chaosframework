/*	
 *	EndianessBufferReaderHelper.cpp
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

#ifdef __linux__
#include <string.h>
#else
#include <string>
#endif
#include "EndianessBufferReaderHelper.h"

using namespace chaos;

    //Static union for check endias of the system
static union {
    uint32_t i;
    char c[4];
} bint ={0x01020304};

EndianessBufferReaderHelper::EndianessBufferReaderHelper() {
    needBigEndian = false;
    isBigEndian = is_big_endian();
}

void EndianessBufferReaderHelper::setNeedBigEndian( bool nb) {
    needBigEndian = nb;
}

int EndianessBufferReaderHelper::is_big_endian(void) {
    return bint.c[0] == 1;
}

void EndianessBufferReaderHelper::swap2byte(char * dest, const char * src) {
    if(isBigEndian == needBigEndian){
        memcpy(dest, src, 2);
    } else {
        dest[0] = src[1];
        dest[1] = src[0];
    }
}

void EndianessBufferReaderHelper::swap4byte(char * dest, const char * src) {
    if(isBigEndian == needBigEndian){
        memcpy(dest, src, 4);
    } else {
        dest[0] = src[3];
        dest[1] = src[2];
        dest[2] = src[1];
        dest[3] = src[0];
    }
}

void EndianessBufferReaderHelper::swap8byte(char * dest, const char * src) {
    if(isBigEndian == needBigEndian){
        memcpy(dest, src, 2);
    } else {
        dest[0] = src[7];
        dest[1] = src[6];
        dest[2] = src[5];
        dest[3] = src[4];
        dest[4] = src[3];
        dest[5] = src[2];
        dest[6] = src[1];
        dest[7] = src[0];
    }
}
