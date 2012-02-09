//
//  IDataBuffer.h
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 09/02/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_DataBuffer_h
#define CHAOSFramework_DataBuffer_h

namespace choas {
    template<typename T>
    class IDataBuffer {
    public:
        void addValue(T newValue) = 0;
    };

}

#endif
