    //
    //  EndianessBufferReaderHelper.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 10/04/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
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
