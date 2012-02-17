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

EndianessBufferReaderHelper::EndianessBufferReaderHelper(){
    needBigEndian = false;
    isBigEndian = is_big_endian();
}

void EndianessBufferReaderHelper::setNeedBigEndian( bool nb) {
    needBigEndian = nb;
}


int16_t EndianessBufferReaderHelper::readInteger(unsigned char* memPtr) {
    uint16_t uInt16;
    memcpy(&uInt16, memPtr, 2);

    if(isBigEndian == needBigEndian){
        return uInt16;
    } else {
        return swap_endian<uint16_t>(uInt16);
    }
}

void EndianessBufferReaderHelper::writeInteger(int16_t uInt16, unsigned char* memPtr) {
    uint16_t tmpUint;
    if(isBigEndian == needBigEndian){
        tmpUint = uInt16;
    } else {
        tmpUint= swap_endian<uint16_t>(uInt16);
    }
    memcpy(memPtr, &tmpUint, 2);
}

int32_t EndianessBufferReaderHelper::readLong(unsigned char* memPtr) {
    int32_t long32;
    memcpy(&long32, memPtr, sizeof(int32_t));
    
    if(isBigEndian == needBigEndian){
        return long32;
    } else {
        return swap_endian<int32_t>(long32);
    }
}

void EndianessBufferReaderHelper::writeLong(int32_t l32, unsigned char* memPtr) {
    int32_t tmpL32;
    if(isBigEndian == needBigEndian){
        tmpL32 = l32;
    } else {
        tmpL32= swap_endian<int32_t>(l32);
    }
    memcpy(memPtr, &tmpL32, sizeof(int32_t));
}

int64_t EndianessBufferReaderHelper::readLongLong(unsigned char* memPtr) {
    int64_t long64;
    memcpy(&long64, memPtr, sizeof(int64_t));
    
    if(isBigEndian == needBigEndian){
        return long64;
    } else {
        return swap_endian<int64_t>(long64);
    }
}

void EndianessBufferReaderHelper::writeLongLong(int64_t l64, unsigned char* memPtr) {
    int64_t tmpL64;
    if(isBigEndian == needBigEndian){
        tmpL64 = l64;
    } else {
        tmpL64= swap_endian<int64_t>(l64);
    }
    memcpy(memPtr, &tmpL64, sizeof(int64_t));
}

double EndianessBufferReaderHelper::readDouble(unsigned char* memPtr) {
    double double64;
    memcpy(&double64, memPtr, 8);
    
    if(isBigEndian == needBigEndian){
        return double64;
    } else {
        return swap_endian<double>(double64);
    }
}

void EndianessBufferReaderHelper::writeDouble(double d64, unsigned char* memPtr) {
    double tmpD64;
    if(isBigEndian == needBigEndian){
        tmpD64 = d64;
    } else {
        tmpD64= swap_endian<double>(d64);
    }
    memcpy(memPtr, &tmpD64, 8);
}
