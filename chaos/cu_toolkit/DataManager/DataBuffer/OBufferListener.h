    //
    //  OBufferListener.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 20/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#ifndef OBufferListener_H
#define OBufferListener_H
#include <vector>
#include <chaos/common/data/CDataWrapper.h>

/*
 Abstract class that implement some method for listen the execution on OBuffer element
 */
namespace chaos{
    class OBuffer;
    class OBufferListener {
        
    public:
        /*
         Called befor the buffer will process the element, if this function
         return false, the process of this element wil be not executed, discardi
         the object
         */
        virtual bool elementWillBeProcessed(int bufferTag, CDataWrapper*)=0;
        virtual bool elementWillBeDiscarded(int bufferTag, CDataWrapper*)=0;
    };
}
#endif

