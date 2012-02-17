/*	
 *	EndianessBufferReaderHelper.h
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

#ifndef EndianessBufferReaderHelper_H
#define EndianessBufferReaderHelper_H

#include <stdint.h>
namespace chaos{
    
    class EndianessBufferReaderHelper {
        bool isBigEndian;
        bool needBigEndian;
        int is_big_endian(void)
        {
        union {
            uint32_t i;
            char c[4];
        } bint = {0x01020304};
        
        return bint.c[0] == 1; 
        }
        
        template <typename T>
        T swap_endian(T u) {
            union {
                T u;
                unsigned char u8[sizeof(T)];
            } source, dest;
            
            source.u = u;
            for (int k = 0; k < sizeof(T); k++)
                dest.u8[k] = source.u8[sizeof(T) - k - 1];
            return dest.u;
        }
    public:
        EndianessBufferReaderHelper();
        void      setNeedBigEndian(bool);
        int16_t   readInteger(unsigned char*);
        int32_t   readLong(unsigned char*);
        int64_t   readLongLong(unsigned char*);
        double    readDouble(unsigned char*);
        void      writeInteger(int16_t, unsigned char*);
        void      writeLong(int32_t, unsigned char*);
        void      writeLongLong(int64_t, unsigned char*);
        void      writeDouble(double, unsigned char*);
    };
}
#endif
